/*
 * Main file for the stm32 part of AutoTux
 *
 * Initially based on the USB-CDC example from ChibiOS
 * I imagine having if not classes and polymorphism,
 * at least a similar naming across different sensor handling files
 * for example ir_setup, us_setup etc.
 * Also common functions for invoking the measurement process.
 * To the SETUP methods you also provide a pointer to a container 
		of the result (array if necessary).
 * it will then be transparent to the using code (main loop)
 * if a callback function is used or not.
 *
 */

#include <stdio.h>
#include <string.h>

#include <ch.h>
#include "hal.h"
#include "test.h"
#include "chprintf.h"
#include "usbcfg.h"
#define CHPRINTF_USE_FLOAT   TRUE

#include "hardwareIR.h"
#include "hardwareUS.h"
#include "hardwareRC.h"
#include "hardwareWE.h"

#define DEBUG_OUTPUT 0

/*
 * Starting point  
 */
int main(void) {
	// Initialize drivers etc
	halInit();
	chSysInit();

	// Initialize IR
	hardwareSetupIR();
	hardwareSetupUS();
	hardwareSetupRC();
	hardwareSetupWE();

 	// Initialize serial over USB
	sduObjectInit(&SDU1);
 	sduStart(&SDU1, &serusbcfg);

  	// Activate USB driver, USB pull-up on D+
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
				/*
				if ((char)charbuf == '\r') {
					chprintf( (BaseSequentialStream *)&SDU1, "\r\n", (char)charbuf);	
				} else {
					chprintf( (BaseSequentialStream *)&SDU1, "%c", (char)charbuf);
				}*/
				// Received a character!
				iterationsSinceActive = 0;
			} else {
				// Timeout. Provide a \0 to keep connection alive.
				//chprintf( (BaseSequentialStream *)&SDU1, "\0");
			}
			hardwareIterationIR(); // TODO: check time
			hardwareIterationUSStart();
			chThdSleepMilliseconds(500);
			hardwareIterationUSEnd();
			hardwareIterationWE();

			// TODO The sensor c file should package the values in a data array.
			// TODO The packet c file should pack it into the right packet format
			// TODO The packet c file should READ and unpack values into the right format
			// TODO These can then be passed to the CONTROL c file that outputs them
			if (DEBUG_OUTPUT) {
				// "\033[F" for going back to previous line
				chprintf( (BaseSequentialStream *)&SDU1, "\033[FTHROTTLE: %4i ", hardwareGetValuesRC(THROTTLE));
				chprintf( (BaseSequentialStream *)&SDU1, "STEERING: %4i ", hardwareGetValuesRC(STEERING));
				chprintf( (BaseSequentialStream *)&SDU1, "WHEEL: %f ", hardwareGetValuesWE());
				chprintf( (BaseSequentialStream *)&SDU1, "US FRONT: %2i \r\n", hardwareGetValuesUS(FRONT));
				chprintf( (BaseSequentialStream *)&SDU1, "US SIDE: %i ", hardwareGetValuesUS(SIDE));
				chprintf( (BaseSequentialStream *)&SDU1, "SIDE_FRONT: %i ", hardwareGetValuesIR(SIDE_FRONT));
				chprintf( (BaseSequentialStream *)&SDU1, "SIDE_REAR: %i ",  hardwareGetValuesIR(SIDE_REAR));
				chprintf( (BaseSequentialStream *)&SDU1, "REAR: %2i ", hardwareGetValuesIR(REAR));
			} else {
				// Normal packet output
				int size = 6;
				char data[size];
				data[0] = (char)hardwareGetValuesUS(FRONT);
				data[1] = (char)hardwareGetValuesUS(SIDE);
				data[2] = (char)hardwareGetValuesIR(SIDE_FRONT);
				data[3] = (char)hardwareGetValuesIR(SIDE_REAR);
				data[4] = (char)hardwareGetValuesIR(REAR);
				data[5] = 0x00; // No checksum for now!

				// Packet structure: size, colon, (bytes), comma
				chprintf((BaseSequentialStream *)&SDU1, "%i:", size);
				for (int c = 0; c < size; c++) {
					chprintf((BaseSequentialStream *)&SDU1, "%c", data[c]);
				}
				chprintf((BaseSequentialStream *)&SDU1, ",");
			}
		}
	}
	return 0;
}
