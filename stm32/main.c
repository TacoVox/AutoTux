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
#define ACTIVITY_LED_TIMEOUT_ITERATIONS 10

// Buffer for received byte
msg_t charbuf;

// Last valid control data bytes
char controlData[CONTROL_DATA_SIZE];

// Initializes sensor thread, drivers etc.
void initialize(void);


//-----------------------------------------------------------------------------
// Implementation - main loop.
//-----------------------------------------------------------------------------


int main(void) {
	initialize();

	static int lastBufferSize = 0;

	// Main loop. Iteration counter for activity LED
  	int iterationsSinceActive = 0;
  	int iterationsSinceRedLED = 0;
	while(true) {

		//---------------------------------------------------------------------
		// LED management
		//---------------------------------------------------------------------
		// TODO: have simple methods like hardwareRedLEDOn, hardwareGreenLEDOn
		// and hardwareLEDIteration() that resets them after N iterations

		// Handles green LED. On if recently recieved bytes.
		if (iterationsSinceActive < ACTIVITY_LED_TIMEOUT_ITERATIONS) {
			// Active. LED on.
			palSetPad(GPIOD, GPIOD_LED4);
			iterationsSinceActive++;
		} else {
			// Timeout. Clear LED and provide a \0 to keep connection alive.
			chprintf( (BaseSequentialStream *)&SDU1, "\0");
			palClearPad(GPIOD, GPIOD_LED4);
		}

		// Handles red LED. On if recently set to 0
	  	if (iterationsSinceRedLED < ACTIVITY_LED_TIMEOUT_ITERATIONS) {
	  		iterationsSinceRedLED++;
			palSetPad(GPIOD, GPIOD_LED5);
	  	} else {
	  		// Off after the given number of iterations
			palClearPad(GPIOD, GPIOD_LED5);
	  	}

		//---------------------------------------------------------------------
		// Receiving part
		//---------------------------------------------------------------------

		// Read bytes until we get a timeout, meaning we have caught up with
		// whatever is sent from the high-level board.
		charbuf = chnGetTimeout(&SDU1, TIME_IMMEDIATE);
		while (charbuf != Q_TIMEOUT) {
			// Received a byte = activity
			iterationsSinceActive = 0;

			// Add the byte to the packet buffer
			appendToBuffer((char)charbuf);

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
				// TODO
				iterationsSinceRedLED = 0;

			}
		}
		// Keep track of buffer size so we only execute the above if anything is new
		lastBufferSize = getPacketBufferSize();


		//---------------------------------------------------------------------
		// Output to hardware
		//---------------------------------------------------------------------

		// Regardless of how it went, controlData contains the latest
		// valid instructions. Output it to hardware
		// TODO: hardwareOutput(controlData);
		// hardwareSetValuesPWM(PWM_OUTPUT_SERVO, angle);
		// hardwareSetValuesPWM(PWM_OUTPUT_ESC, dir);

		//chprintf((BaseSequentialStream*) &SDU1, "CONTROL 1: %2x", controlData[0]);
		//chprintf((BaseSequentialStream*) &SDU1, "CONTROL 2: %2x", controlData[1]);


		//---------------------------------------------------------------------
		// Sending part
		//---------------------------------------------------------------------

		if (DEBUG_OUTPUT) {
			sensorDebugOutput((BaseSequentialStream*) &SDU1);
		} else {
			// Send a sensor data packet. Fill a char[] with sensor values.
			int size = 5;
			char data[size];
			getSensorData(data);

			// Send to SDU1
			sendPacket(data, 5, (BaseSequentialStream*) &SDU1);
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
