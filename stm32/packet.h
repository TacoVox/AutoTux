/*
 * packet.h
 */

#ifndef PACKET_H_
#define PACKET_H_

#define CONTROL_DATA_SIZE 2
#define CONTROL_DATA_PACKET_SIZE 5

typedef enum {PACKET_OK, NO_PACKET} PACKET_STATUS;


/**
 * Sends a packet based on the data array to the serial connection
 * NOTE: provide the size of the actual data array!
 */
void sendPacket(char* data, int dataSize, BaseSequentialStream* SDU);

/**
 * Appends char to buffer. Will automatically discard half the buffer
 * (the oldest part) if buffer size is reached. (The buffer is cleared
 * by the createPacketFromBuffer function on successful packet read).
 */
void appendToBuffer(char latestByte);


/**
 * Tries to create a packet from buffer, starting from the end of the buffer
 * and going back until the first valid packet is instantiated. If successful,
 * clears the buffer and returns PACKET_OK. Otherwise NO_PACKET.
 * Initialize the char* with CONTROL_DATA_SIZE.
 */
PACKET_STATUS readPacketFromBuffer(char* data);


/**
 * Returns the current buffer size.
 */
int getPacketBufferSize(void);


#endif /* PACKET_H_ */
