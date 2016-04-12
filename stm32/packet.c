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

char generateChecksumByte(char* data, int dataSize);



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
	chprintf(SDU, "%c", generateChecksumByte(data, size));
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

		// Copy BUFFER_SIZE / 2 characters STARTING AT
		// the address buffer + BUFFER_SIZE / 2 to temporary buffer
		char tmpBuffer[BUFFER_SIZE / 2];
		strncpy(tmpBuffer, buffer + BUFFER_SIZE / 2, BUFFER_SIZE / 2);

		// Copy back
		strncpy(buffer, tmpBuffer, BUFFER_SIZE / 2);
		bufferCounter = BUFFER_SIZE / 2;
	}
}


/**
 * Tries to read a packet from buffer, starting from the end of the buffer
 * and going back until the first valid packet is instantiated. If successful,
 * clears the buffer and returns PACKET_OK. Otherwise NO_PACKET.
 * Initialize the char* with CONTROL_DATA_SIZE.
 */
PACKET_STATUS readPacketFromBuffer(char* data) {
	(void) data;

	// ARBITRARY RETURN, NOT IMPLEMENTED
	if (bufferCounter > (BUFFER_SIZE / 2) + 2)
		return PACKET_OK;
	else
		return NO_PACKET;
}


int getPacketBufferSize(void) {
	return bufferCounter;
}


//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------


char generateChecksumByte(char* data, int dataSize) {
	(void) data;
	(void) dataSize;
	return 0x00;
}

