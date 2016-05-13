/** @file	serialConnection.c
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


// General includes
#include <stdio.h>
#include <string.h>

// ChibiOS includes
#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include <chibiconf/usbcfg.h>

// Local includes
#include "../autotuxconfig.h"
#include "../sensorInput.h"
#include "../controlOutput.h"
#include "packetHandler.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


static void initializeUSB(void);
static THD_FUNCTION(serialThread, arg);

/**
 * Working area for the serial thread. Currently 300 bytes stack size.
 */
static THD_WORKING_AREA(serialThreadWorkingArea, 300);


//-----------------------------------------------------------------------------
// Public interface
//-----------------------------------------------------------------------------


/**
 * @brief Starts the serial connection.
 *
 * First initializes the USB serial driver and then starts the thread
 * dedicated to serial communication.
 */
void serialConnectionStart(void) {
	// Initialize USB serial driver and start thread
	initializeUSB();
	(void)chThdCreateStatic(serialThreadWorkingArea, sizeof(serialThreadWorkingArea),
						  NORMALPRIO, serialThread, NULL);
}


//-----------------------------------------------------------------------------
// Implementation. The static functions below are inaccessible to other modules
//-----------------------------------------------------------------------------


/**
 * Main loop for the serial thread.
 */
static THD_FUNCTION(serialThread, arg) {
	(void) arg;

	// Buffer for last received byte.
	msg_t charbuf;

	// Counter for iterations without valid packet received.
	int iterationsWithoutReceive = 0;

	// Counter for bytes received in the current iteration.
	int bytesReceived = 0;

	// Determines if USB connected - means we can send data to USB without fear of filling the output buffer
	bool usbConnected = false;

	// Whether we are receiving valid packets from Odroid regularly - means we should trust the packets
	// to control the car
	bool connectionLinkActive = false;

	// Sensor data bytes to be sent to the Odroid.
	unsigned char sensorData[SENSOR_DATA_BYTES];

	// Time of the iteration. Used to adapt the sleeping
	unsigned int itTime = 0;

	// Latest received control bytes. Should only be forwarded on valid packet.
	unsigned char controlData[CONTROL_DATA_BYTES] = DEFAULT_CONTROL_BYTES;

	// Main serial connection loop.
	while(true) {
		itTime = ST2MS(chVTGetSystemTime());
		

		//---------------------------------------------------------------------
		// Reset all serial related LEDs
		//---------------------------------------------------------------------
		palClearPad(GPIOD, GPIOD_LED4); // Green
		palClearPad(GPIOD, GPIOD_LED5); // Red
		palClearPad(GPIOD, GPIOD_LED3); // Orange


		//---------------------------------------------------------------------
		// Receiving part
		//---------------------------------------------------------------------

		// Read bytes until we get a timeout, meaning we have caught up with
		// whatever is sent from the high-level board. Abort if we reach byte limit.
		charbuf = chnGetTimeout(&SDU1, TIME_IMMEDIATE);
		bytesReceived = 0;
		while (charbuf != Q_TIMEOUT && charbuf != Q_RESET &&
				bytesReceived < MAX_RECEIVE_BYTES_IN_ITERATION) {

			if (CALIBRATION_MODE == 1) {
				// CALIBRATION SPECIAL
				if ((unsigned char)charbuf == 'n') {
					controlData[CONTROL_BYTE_SPEED] = 1;
				} else if ((unsigned char)charbuf == 'f') {
					controlData[CONTROL_BYTE_SPEED] = 2;
				} if ((unsigned char)charbuf == 'b') {
					controlData[CONTROL_BYTE_SPEED] = 0;
				}
				chprintf((BaseSequentialStream*)&SDU1, "\nCALIBRATION OUT: %i", CTRL_OUT_SPEED_PULSEWIDTHS[controlData[CONTROL_BYTE_SPEED]]);
				controlOutputSetData(controlData);
			}

			// Received another byte
			bytesReceived++;

			// Add the byte to the packet buffer
			packetHandlerAddToRcvBuffer((unsigned char)charbuf);

			// Read the next byte - but first wait a bit, the USB-serial driver
			// tends to hang if we read to soon.
			chThdSleepMicroseconds(50);
			charbuf = chnGetTimeout(&SDU1, TIME_IMMEDIATE);
		}

		if (charbuf == Q_RESET) {
			// Disconnected - light up red LED
			palSetPad(GPIOD, GPIOD_LED5);
			usbConnected = FALSE;
		} else {
			usbConnected = TRUE;
		}

		// Received all bytes available from serial. Time to try to instantiate
		// a packet, provided we did receive something this iteration
		bool receivedValidPacket = FALSE;
		if (packetHandlerGetBufferSize() >= CONTROL_DATA_PACKET_SIZE &&
				bytesReceived > 0) {
			if (packetHandlerReadPacketFromBuffer(controlData) == PACKET_OK) {

				// Valid packet - green LED
				palSetPad(GPIOD, GPIOD_LED4);
				receivedValidPacket = TRUE;
				iterationsWithoutReceive = 0;

				// Inform controlOutput of the new data
				controlOutputSetData(controlData);

			} else {
				if (!DEBUG_OUTPUT) {
					// Broken packet or garbage - orange LED
					palSetPad(GPIOD, GPIOD_LED3);
				} else {
					// DEBUG MODE - any received bytes will be seen as active connection
					receivedValidPacket = TRUE;
					iterationsWithoutReceive = 0;

				}
			}
		}

		// Increase counter if we didn't receive a valid packet
		if (!receivedValidPacket &&
				iterationsWithoutReceive <= MAX_ITERATIONS_WITHOUT_RECEIVE) {
			iterationsWithoutReceive++;
		}

		// We've got to try not to fill the USB output buffer if we have no connection.
		// Therefore, we check if the connection seems active before sending
		if (iterationsWithoutReceive < MAX_ITERATIONS_WITHOUT_RECEIVE) {
			connectionLinkActive = TRUE;
		} else {
			connectionLinkActive = FALSE;
			// Connection problems - light up red LED
			palSetPad(GPIOD, GPIOD_LED5);
		}


		//---------------------------------------------------------------------
		// Sending part
		//---------------------------------------------------------------------
		if (usbConnected && connectionLinkActive) {
			if (!DEBUG_OUTPUT) {
				// Normal output
				// Send a sensor data packet. Fill data array with sensor values.
				sensorInputGetData(sensorData);
				// Send to SDU1
				packetHandlerSend(sensorData, SENSOR_DATA_BYTES, (BaseSequentialStream*) &SDU1);
			} else {
				// Sends debug output to serial instead of a normal packet.
				sensorInputDebugOutput((BaseSequentialStream*) &SDU1);
			}
		}
		
		if (ST2MS(chVTGetSystemTime()) > itTime) {
			// itTime - ST2MS(chTimeNow()) is the time in MS of this iteration.
			// Sleep 100 - this value to get 10Hz. 67 to get 15 hz.
			 chThdSleepMilliseconds(50 - (itTime - ST2MS(chVTGetSystemTime())));
		} else {
			// System timer recently overflowed, rare but can happen. Sleep a fixed amount
			chThdSleepMilliseconds(50);
		}
	}
}


/**
 * Initializes serial driver for USB
 */
static void initializeUSB(void) {
 	// Initialize serial over USB
	sduObjectInit(&SDU1);
 	sduStart(&SDU1, &serusbcfg);

  	// Activate USB driver. The delay means that if the device is reset, it will
 	// be unavailable to the host for a while, and then reattached.
	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1500);
	usbStart(serusbcfg.usbp, &usbcfg);
  	usbConnectBus(serusbcfg.usbp);
}
