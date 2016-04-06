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

#include "hardwareIR.h"
#include "hardwareUS.h"
#include "hardwareRC.h"

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
				chprintf( (BaseSequentialStream *)&SDU1, "\0");
			}
			hardwareIterationIR(); // TODO: check time
			hardwareIterationUSStart();
			chThdSleepMilliseconds(100);
			hardwareIterationUSEnd();
			//hardwareIterationRC();

			chprintf( (BaseSequentialStream *)&SDU1, "\033[FTHROTTLE: %i ", hardwareGetValuesRC(THROTTLE));
			chprintf( (BaseSequentialStream *)&SDU1, "STEERING: %i ", hardwareGetValuesRC(STEERING));
			chprintf( (BaseSequentialStream *)&SDU1, "US FRONT: %i ", hardwareGetValuesUS(FRONT));
			chprintf( (BaseSequentialStream *)&SDU1, "US SIDE: %i \r\n", hardwareGetValuesUS(SIDE));
			chprintf( (BaseSequentialStream *)&SDU1, "SIDE_FRONT: %i ", hardwareGetValuesIR(SIDE_FRONT));
			chprintf( (BaseSequentialStream *)&SDU1, "SIDE_REAR: %i ",  hardwareGetValuesIR(SIDE_REAR));
			chprintf( (BaseSequentialStream *)&SDU1, "REAR: %i \r", hardwareGetValuesIR(REAR));
		}
	}
	return 0;
}
