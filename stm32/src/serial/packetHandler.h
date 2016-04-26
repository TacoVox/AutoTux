/** @file	packetHandler.c
 * 	@brief	Handles sending and receiving packets, manages a receive buffer.
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

#ifndef PACKETHANDLER_H_
#define PACKETHANDLER_H_

/**
 * Actual data bytes in the control packet
 */
#define CONTROL_DATA_BYTES 2

/**
 * Size of the whole control data packet including header and end delimiter
 */
#define CONTROL_DATA_PACKET_SIZE 6

/**
 * Actual data bytes in the sensor data packet
 */
#define SENSOR_DATA_BYTES 11

/**
 * Returned from the packet reading function.
 */
typedef enum {PACKET_OK, NO_PACKET} PACKET_STATUS;


/**
 * @brief Sends a packet based on the data array to the serial connection
 *
 * NOTE: provide the size of the actual data array!
 */
void packetHandlerSend(unsigned char* data, int dataSize, BaseSequentialStream* SDU);

/**
 * @brief Append char to receive buffer.
 *
 * Will automatically discard half the buffer (the oldest part) if buffer size
 * is reached. (The buffer is cleared by the createPacketFromBuffer function on
 * successful packet read).
 */
void packetHandlerAddToRcvBuffer(unsigned char latestByte);

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
PACKET_STATUS packetHandlerReadPacketFromBuffer(unsigned char* data);

/**
 * Getter for the currently used buffer size.
 */
int packetHandlerGetBufferSize(void);


#endif /* PACKETHANDLER_H_ */
