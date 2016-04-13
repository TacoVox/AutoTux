/**
 * Main file for the stm32 part of AutoTux.
 * Some of the files such as usbcfg.*, chconf, mcuconf and halconf are from the ChibiOS
 * examples for stm32. The config files are tailored for our particular needs.
 * Also note this from the ChibiOS example readme:
 *
 * "Some files used by the demo are not part of ChibiOS/RT but are copyright of
 * ST Microelectronics and are licensed under a different license.
 * Also note that not all the files present in the ST library are distributed
 * with ChibiOS/RT, you can find the whole library on the ST web site."
 */


// General includes
#include <stdio.h>
#include <string.h>
// ChibiOS includes
#include <ch.h>
#include <chprintf.h>
#include <hal.h>

// Local includes
#include "usbcfg.h"
#include "sensorInput.h"
#include "packet.h"


// TODO: Move to output
#include "hardwarePWM.h"
// TODO: should also be in output module
#include "hardwareRC.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


#define DEBUG_OUTPUT 0


// Buffer for received byte
msg_t charbuf;

// Last valid control data bytes
// TODO: MOVE TO OUTPUT HARDWARE FILE
unsigned char controlData[CONTROL_DATA_SIZE];


// Initializes sensor thread, drivers etc.
void initialize(void);


//-----------------------------------------------------------------------------
// Implementation - main loop.
//-----------------------------------------------------------------------------


int main(void) {
	initialize();

	static int lastBufferSize = 0;


	// TODO: MOVE TO OUTPUT HARDWARE FILE
	controlData[CONTROL_BYTE_SPEED] = SPEED_PULSEWIDTHS[SPEED_STOP];
	controlData[CONTROL_BYTE_ANGLE] = WHEELS_CENTERED_ANGLE;


	// Main loop. Iteration counter for activity LED
	while(true) {

		//---------------------------------------------------------------------
		// Reset all LEDS
		//---------------------------------------------------------------------
		palClearPad(GPIOD, GPIOD_LED4);
		palClearPad(GPIOD, GPIOD_LED5);

		//---------------------------------------------------------------------
		// Receiving part
		//---------------------------------------------------------------------

		// Read bytes until we get a timeout, meaning we have caught up with
		// whatever is sent from the high-level board.
		charbuf = chnGetTimeout(&SDU1, TIME_IMMEDIATE);
		while (charbuf != Q_TIMEOUT) {
			// Received a byte = activity
			//iterationsSinceActive = 0;

			// Add the byte to the packet buffer
			appendToBuffer((unsigned char)charbuf);

			// Read the next byte - but first wait a bit, the USB-serial driver
			// tends to hang if we read to soon.
			chThdSleepMicroseconds(50);
			charbuf = chnGetTimeout(&SDU1, TIME_IMMEDIATE);
		}

		// Received all bytes available from serial. Time to try to instantiate
		// a packet, provided the buffer has some content now
		if (getPacketBufferSize() >= CONTROL_DATA_PACKET_SIZE &&
				getPacketBufferSize() > lastBufferSize) {
			// TODO: Light up red LED if failed to instantiate packet
			if (readPacketFromBuffer(controlData) == PACKET_OK) {
				// Valid packet - green LED
				palSetPad(GPIOD, GPIOD_LED4);
			} else {
				// Broken packet or garbage - red LED
				palSetPad(GPIOD, GPIOD_LED5);
			}
		}
		// Keep track of buffer size so we only execute the above if anything is new
		lastBufferSize = getPacketBufferSize();


		//---------------------------------------------------------------------
		// Output to hardware
		//---------------------------------------------------------------------

		// Regardless of how it went, controlData at this point contains the latest
		// valid instructions. Output it to hardware.
		// TODO: hardwareOutput(controlData);

		// Speed controlled by int corresponding to SPEED enum in hardware config
		hardwareSetValuesPWM(PWM_OUTPUT_ESC, controlData[CONTROL_BYTE_SPEED]);

		// Wheel angle: 90 degress +- ~30 degrees.
		hardwareSetValuesPWM(PWM_OUTPUT_SERVO, controlData[CONTROL_BYTE_ANGLE]);


		//---------------------------------------------------------------------
		// Sending part
		//---------------------------------------------------------------------

		if (DEBUG_OUTPUT) {
			sensorDebugOutput((BaseSequentialStream*) &SDU1);
		} else {
			// Send a sensor data packet. Fill a char[] with sensor values.
			int size = 6;
			unsigned char data[size];
			getSensorData(data);

			// Send to SDU1
			sendPacket(data, size, (BaseSequentialStream*) &SDU1);
		}

		chThdSleepMilliseconds(100);
	}
	return 0;
}


/**
 * Initializes sensor thread, drivers etc
 */
void initialize() {
	// Initialize drivers etc
	halInit();
	chSysInit();

	// Initialize sensor settings
	sensorSetup();
	hardwareSetupPWM();

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
