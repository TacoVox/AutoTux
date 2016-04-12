/*
 * Main file for the stm32 part of AutoTux
 */

#include <stdio.h>
#include <string.h>

#include <ch.h>
#include "chprintf.h"
#include "hal.h"

#include "usbcfg.h"


// Input
#include "sensorInput.h"

// Output
#include "hardwarePWM.h"
// TODO: should also be in output module
#include "hardwareRC.h"


#define DEBUG_OUTPUT 1

/*
 * Starting point  
 */
int main(void) {
	int dir = 1; // 1 = neutral, 0 = back, 2 = forward. for testing
	int angle = 1; // 1 = center, 0 = left, 2 right. for testing

	// Initialize drivers etc
	halInit();
	chSysInit();

	// Initialize sensor settings
	sensorSetup();
	hardwareSetupPWM();

 	// Initialize serial over USB
	sduObjectInit(&SDU1);
 	sduStart(&SDU1, &serusbcfg);

  	// Activate USB driver
	// Delay means that if device is reset, it will be unavailable to the
	// host for a while, and then reattached
	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1500);
	usbStart(serusbcfg.usbp, &usbcfg);
  	usbConnectBus(serusbcfg.usbp);

	// Main loop
  	int iterationsSinceActive = 0;
	while(true) {
		msg_t charbuf;
		int active = 1; // This can later be switched on timeout!

		while (active) {
			if (iterationsSinceActive < 3) {
				// Active. LED on.
				palSetPad(GPIOD, GPIOD_LED4);
				iterationsSinceActive++;
			} else {
				palClearPad(GPIOD, GPIOD_LED4);
			}

			charbuf = chnGetTimeout(&SDU1, 100);
			if (charbuf != Q_TIMEOUT) {
				// Receiving code: if buffer size under packet size,
				// keep waiting. Also KEEP PROCESSING if accumulated buffer size
				// can contain several packets.


				if ((char)charbuf == '\r') {
					chprintf( (BaseSequentialStream *)&SDU1, "\r\n", (char)charbuf);	
				} else {
					chprintf( (BaseSequentialStream *)&SDU1, "%c", (char)charbuf);
				}

				// Received a character!
				if ((char)charbuf == 'f')
					dir = 2;
				if ((char)charbuf == 's')
					dir = 1;
				if ((char)charbuf == 'b')
					dir = 0;
				if ((char)charbuf == 'l')
					angle = 0;
				if ((char)charbuf == 'c')
					angle = 1;
				if ((char)charbuf == 'r')
					angle = 2;

				iterationsSinceActive = 0;
			} else {
				// Timeout. Provide a \0 to keep connection alive.
				chprintf( (BaseSequentialStream *)&SDU1, "\0");
			}



			hardwareSetValuesPWM(PWM_OUTPUT_SERVO, angle);
			hardwareSetValuesPWM(PWM_OUTPUT_ESC, dir);

			// TODO The sensor c file should package the values in a data array.
			// TODO The packet c file should pack it into the right packet format
			// TODO The packet c file should READ and unpack values into the right format
			// TODO These can then be passed to the CONTROL c file that outputs them
			if (DEBUG_OUTPUT) {
				sensorDebugOutput((BaseSequentialStream*) &SDU1);
			} else {
				// Normal packet output
				int size = 6;
				char data[size];
				getSensorData(data);

				// Packet structure: size, colon, (bytes), comma
				chprintf((BaseSequentialStream *)&SDU1, "%i:", size);
				for (int c = 0; c < size; c++) {
					chprintf((BaseSequentialStream *)&SDU1, "%c", data[c]);
				}
				chprintf((BaseSequentialStream *)&SDU1, ",");
			}

			chThdSleepMilliseconds(100);
		}
	}
	return 0;
}
