/*
 * Takes care of sensor input for the stm32 part of AutoTux.
 * Returns a char array of the data that should be passed to the
 * high-level board.
 */


#include <ch.h>
#include <chprintf.h>
#include <hal.h>
#include <string.h>

#include "packet.h"



//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


#define BUFFER_SIZE 128

char buffer[BUFFER_SIZE];
int bufferCounter = 0;

void strnshift(char* bufferString, int size, int n);
char generateChecksum(char* data, int dataSize);


//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------


/**
 * Sends a packet based on the data array to the serial connection
 * NOTE: provide the size of the actual data array!
 */
void sendPacket(char* data, int size, BaseSequentialStream* SDU) {
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
 * Appends char to buffer. Will automatically discard half the buffer
 * (the oldest part) if buffer size is reached. (The buffer is cleared
 * by the createPacketFromBuffer function on successful packet read).
 */
void appendToBuffer(char latestByte) {
	buffer[bufferCounter] = latestByte;
	bufferCounter++;
	if (bufferCounter > BUFFER_SIZE) {
		// Shift the buffer back in the array, discarding the oldest half
		strnshift(buffer, BUFFER_SIZE, BUFFER_SIZE / 2);
		bufferCounter = BUFFER_SIZE / 2;
	}
}


/**
 * Tries to read a packet from buffer, starting from the end of the buffer
 * and going back until the first valid packet is instantiated. If successful,
 * shifts the buffer and returns PACKET_OK. Otherwise NO_PACKET.
 * Initialize the char* with CONTROL_DATA_SIZE.
 * NOTE that implementation currently relies on the packet size being one digit.
 */
PACKET_STATUS readPacketFromBuffer(char* data) {
	bool packetFound = FALSE;
	// If packet seems to be found, values are put here temporarily before packet verified
	char packetTest[CONTROL_DATA_SIZE];

	for (int endCharPos = bufferCounter - 1; endCharPos > CONTROL_DATA_PACKET_SIZE &&
			!packetFound; endCharPos--) {
		// See if char at endCharPos is ASCII comma
		if (buffer[endCharPos] == ',') {
			// If so, see if we can find a digit (packet size) and colon at the
			// expected start of this possible packet
			if (buffer[endCharPos - CONTROL_DATA_PACKET_SIZE] ==
					'0' + CONTROL_DATA_SIZE + 1 && // ASCII for BYTE COUNT + CHECKSUM BYTE
					buffer[endCharPos - CONTROL_DATA_PACKET_SIZE + 1] == ':') {

				int dataStartPos = endCharPos - CONTROL_DATA_PACKET_SIZE + 2;

				// Read values
				for (int i = 0; i < CONTROL_DATA_SIZE; i++)
					packetTest[i] = buffer[dataStartPos + i];

				// Finally, see if the checksum is correct
				if (buffer[endCharPos - 1] == generateChecksum(packetTest, CONTROL_DATA_SIZE)) {

					// Shift the buffer, all content up to and including packet is discarded
					strnshift(buffer, BUFFER_SIZE, endCharPos);
					bufferCounter = endCharPos;

					// Update the data array provided as an argument
					for (int i = 0; i < CONTROL_DATA_SIZE; i++)
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


int getPacketBufferSize(void) {
	return bufferCounter;
}


//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------


/**
 * Shifts the string n positions to the left.
 */
void strnshift(char* bufferString, int size, int n) {
	for (int i = 0; i < n && i + n < size; i++) {
		bufferString[i] = bufferString[i + n];
 	}
}


char generateChecksum(char* data, int dataSize) {
	(void) data;
	(void) dataSize;
	return 0x00;
}

