/**
 * Serial connection.
 */


// General includes
#include <stdio.h>
#include <string.h>
// ChibiOS includes
#include <ch.h>
#include <chprintf.h>
#include <hal.h>
#include "usbcfg.h"

// Local includes
#include "autotuxconfig.h"
#include "sensorInput.h"
#include "controlOutput.h"
#include "packetHandler.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


// Initializes the USB serial driver.
void initializeUSB(void);

// Initializes everything related to sensors and control output
void initializeHW(void);


//-----------------------------------------------------------------------------
// Implementation - main loop.
//-----------------------------------------------------------------------------


int serialConnectionLoop(void) {
	initializeUSB();
	initializeHW();

	// Buffer for last received byte.
	msg_t charbuf;

	// Last received control data bytes. Initialized to car stopped, wheels centered
	unsigned char controlData[CONTROL_DATA_BYTES] = DEFAULT_CONTROL_BYTES;

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

	// TODO: MOVE TO OUTPUT HARDWARE FILE
	// Car initial state: stopped, wheels centered
	controlOutputStopCenter();

	// Time of the iteration. Used to adapt the sleeping
	int itTime = 0;

	// Main serial connection loop.
	while(true) {
		itTime = ST2MS(chTimeNow());
		
		//---------------------------------------------------------------------
		// Reset all LEDS
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
		}


		//---------------------------------------------------------------------
		// Output to hardware
		//---------------------------------------------------------------------

		// Unless we received no data for a number of iterations or too much data at once,
		// controlData contains the latest valid instructions. Output them to hardware.
		if (connectionLinkActive && bytesReceived <= MAX_RECEIVE_BYTES_IN_ITERATION) {
			// Forward control data to hardware
			controlOutput(controlData);
		} else {
			// Serial connection rules violated. Stop the car and center wheels!
			// TODO: MAYBE it would be good style here to overwrite controlData to stop-center values?
			// On the other hand, connectionLinkActive should only be TRUE again if a new valid
			// packet is received.
			controlOutputStopCenter();
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
		
		if (ST2MS(chTimeNow()) > itTime) {
			// itTime - ST2MS(chTimeNow()) is the time in MS of this iteration.
			// Sleep 100 - this value to get 10Hz.
			 chThdSleepMilliseconds(100 - (itTime - ST2MS(chTimeNow()));
		} else {
			// System timer recently overflowed, rare but can happen. Sleep a fixed amount
			chThdSleepMilliseconds(95);
		}
	}
	return 0;
}


/**
 * Initializes USB serial driver
 */
void initializeUSB(void) {
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


/**
 * Initializes everything related to sensors and control output
 */
void initializeHW(void) {
	// Initialize sensor settings
	sensorInputSetup();
	controlOutputSetup();
}
