/*
 * packet.h
 */

#ifndef PACKETHANDLER_H_
#define PACKETHANDLER_H_

#define CONTROL_DATA_BYTES 2	// Actual data bytes
#define CONTROL_DATA_PACKET_SIZE 6 // Size of whole packet
#define SENSOR_DATA_BYTES 11 // Actual data bytes

typedef enum {PACKET_OK, NO_PACKET} PACKET_STATUS;


/**
 * Sends a packet based on the data array to the serial connection
 * NOTE: provide the size of the actual data array!
 */
void packetHandlerSend(unsigned char* data, int dataSize, BaseSequentialStream* SDU);

/**
 * Appends char to buffer. Will automatically discard half the buffer
 * (the oldest part) if buffer size is reached. (The buffer is cleared
 * by the createPacketFromBuffer function on successful packet read).
 */
void packetHandlerAddToRcvBuffer(unsigned char latestByte);


/**
 * Tries to create a packet from buffer, starting from the end of the buffer
 * and going back until the first valid packet is instantiated. If successful,
 * clears the buffer and returns PACKET_OK. Otherwise NO_PACKET.
 * Initialize the char* with CONTROL_DATA_SIZE.
 */
PACKET_STATUS packetHandlerReadPacketFromBuffer(unsigned char* data);


/**
 * Returns the current buffer size.
 */
int packetHandlerGetBufferSize(void);


#endif /* PACKETHANDLER_H_ */
