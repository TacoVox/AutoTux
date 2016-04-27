/** @file	serialConnection.h
 * 	@brief	Handles the serial connection.
 *
 * 	Reads sensor data through sensorInput and uses packetHandler to send a packet
 * 	with sensor data.
 *
 * 	Every byte received gets appended to packetHandler's buffer. When we think we
 * 	have enough bytes to parse a control data packet, we ask packetHandler to do
 * 	so and if it says the parsing was successful, we call a setter in
 * 	controlOutput to forward the control data values to the hardware.
 *
 * 	Because the USB serial output buffer is limited, and writing to the serial
 * 	driver when the buffer is full is blocking the thread, we take care not to
 * 	send anything if we haven't received any (valid) packets for a while. However,
 * 	since the serial connection is now running in it's own thread, it's really
 * 	not a big problem if this thread is blocking anymore.
 *
 * 	And in case any problems occur with the communication, the controlOutput module
 * 	will make sure that if no valid values are received for a while, the car will
 * 	stop.
 */


#ifndef SERIALCONNECTION_H_
#define SERIALCONNECTION_H_


/**
 * Starts the serial connection in it's own thread.
 */
void serialConnectionStart(void);


#endif /* SERIALCONNECTION_H_ */
