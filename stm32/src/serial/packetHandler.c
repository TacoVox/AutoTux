/** @file	packetHandler.c
 * 	@brief	Handles sending and parsing of received packets, manages a receive buffer.
 *
 * 	When receiving bytes, they are put into the receive buffer. If the buffer
 * 	gets full, it gets shifted so the oldest half gets discarded. The using
 * 	code in the serial connection will call packetHandlerReadPacketFromBuffer
 * 	when appropriate. This will go through the buffer from the most recent end,
 * 	trying to find the end of a packet, it will then find the rest and try to
 * 	initialize and validate the packet. When it has successfully initiated a
 * 	packet, everything older than the end of that packet is discarded.
 * 	If no packet found, the buffer is left untouched.
 *
 * 	We track the active characters in the buffer with a counter, so when we shift
 * 	the values back half a buffer size, we never "clear" the "free" side of buffer
 * 	by changing the values to '\0' or similar.
 */

#include <ch.h>
#include <chprintf.h>
#include <hal.h>
#include <string.h>

#include "packetHandler.h"
#include "../autotuxconfig.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


static void strnshift(unsigned char* bufferString, int size, int n);
static char generateChecksum(unsigned char* data, int dataSize);


/**
 * The internal receive buffer size.
 */
#define BUFFER_SIZE 128

/**
 * The internal receive buffer.
 */
static unsigned char buffer[BUFFER_SIZE];

/**
 * Points to the next free position in the receive buffer
 *
 * (Which also implies it's value represents the currently used characters of
 * the buffer).
 */
static int bufferCounter = 0;


//-----------------------------------------------------------------------------
// Public interface
//-----------------------------------------------------------------------------


/**
 * @brief Sends a packet based on the data array to the serial connection
 *
 * NOTE: provide the size of the actual data array!
 */
void packetHandlerSend(unsigned char* data, int size, BaseSequentialStream* SDU) {
	// Packet structure: size, colon, (bytes), checksum byte, comma.
	// Note: size + 1 because we also append a checksum XOR byte!
	chprintf(SDU, "%i:", size + 1);
	for (int c = 0; c < size; c++) {
		// Send each byte in data array
		chprintf(SDU, "%c", data[c]);
	}
	// Send the checksum and end delimiter
	chprintf(SDU, "%c", generateChecksum(data, size));
	chprintf(SDU, ",");
}


/**
 * @brief Append char to receive buffer.
 *
 * Will automatically discard half the buffer (the oldest part) if buffer size
 * is reached. (The buffer is cleared by the createPacketFromBuffer function on
 * successful packet read).
 */
void packetHandlerAddToRcvBuffer(unsigned char latestByte) {
	buffer[bufferCounter] = latestByte;
	bufferCounter++;
	if (bufferCounter > BUFFER_SIZE) {
		// Shift the buffer back in the array, discarding the oldest half
		strnshift(buffer, BUFFER_SIZE, BUFFER_SIZE / 2);
		bufferCounter = BUFFER_SIZE / 2;
	}
}

/**
 * @brief Tries to read a packet from the receive buffer.
 *
 * The function starts from the end of the buffer and moves backward until the
 * first valid packet is instantiated. If successful, shifts the buffer (discarding
 * anything up until and including the packet end) and returns PACKET_OK.
 * Otherwise NO_PACKET.
 *
 * Initialize the char* with CONTROL_DATA_SIZE.
 * NOTE that implementation currently relies on the packet size being one digit.
 */
PACKET_STATUS packetHandlerReadPacketFromBuffer(unsigned char* data) {
	bool packetFound = FALSE;
	// If packet seems to be found, values are put here temporarily before packet verified
	unsigned char packetTest[CONTROL_DATA_BYTES];
	int highestPossibleEndPos = bufferCounter - 1;
	int lowestPossibleEndPos = CONTROL_DATA_PACKET_SIZE - 1;

	for (int endCharPos = highestPossibleEndPos; endCharPos >= lowestPossibleEndPos &&
			!packetFound; endCharPos--) {
		// See if char at endCharPos is ASCII comma
		if (buffer[endCharPos] == ',') {
			// If so, see if we can find a digit (packet size) and colon at the
			// expected start of this possible packet
			int packetStartPos = endCharPos - (CONTROL_DATA_PACKET_SIZE  - 1);

			// ASCII for the digit representing BYTE COUNT + CHECKSUM BYTE
			if (buffer[packetStartPos] == '0' + CONTROL_DATA_BYTES + 1 &&
					buffer[packetStartPos + 1] == ':') {

				int dataStartPos = packetStartPos + 2;

				// Read values
				for (int i = 0; i < CONTROL_DATA_BYTES; i++)
					packetTest[i] = buffer[dataStartPos + i];

				// Finally, see if the checksum is correct
				if (buffer[endCharPos - 1] == generateChecksum(packetTest, CONTROL_DATA_BYTES)) {

					// Shift the buffer, all content up to and including packet is discarded
					strnshift(buffer, BUFFER_SIZE, endCharPos);
					bufferCounter = endCharPos;

					// Update the data array provided as an argument
					for (int i = 0; i < CONTROL_DATA_BYTES; i++)
						data[i] = packetTest[i];

					// Breaks loop and makes sure to return success
					packetFound = true;

				}
			}  // If any of these if statements are not true, keep moving backwards in the buffer
		} // until packet successfully read OR we've reached the beginning of the buffer
	}

	if (packetFound) {
		return PACKET_OK;
	} else {
		return NO_PACKET;
	}
}

/**
 * Getter for the currently used buffer size.
 */
int packetHandlerGetBufferSize(void) {
	return bufferCounter;
}


//-----------------------------------------------------------------------------
// Implementation. The static functions below are inaccessible to other modules
//-----------------------------------------------------------------------------


/**
 * Shifts the string n positions to the left.
 */
static void strnshift(unsigned char* bufferString, int size, int n) {
	for (int i = 0; i < n && i + n < size; i++) {
		bufferString[i] = bufferString[i + n];
 	}
}

/**
 * Generates a checksum by XORing all data bytes.
 */
static char generateChecksum(unsigned char* data, int dataSize) {
	unsigned char checksum = 0;
	for (int i = 0; i < dataSize; i++) {
		checksum ^= data[i];
	}
	return checksum;
}
