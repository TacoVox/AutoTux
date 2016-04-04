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

#include "ch.h"
#include "hal.h"
#include "test.h"
#include "chprintf.h"
#include "usbcfg.h"

#include "autotuxhardware.h"
#include "hardwareIR.h"

/*
 * Starting point  
 */
int main(void) {
	// Initialize drivers etc
	halInit();
	chSysInit();

	// Initialize IR
	initializeIR();

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
	while(true) {
		msg_t charbuf;
		int exit = 0; // This can later be switched on timeout!

		while (!exit) {
			// LED on
			palSetPad(GPIOD, GPIOD_LED4);
	
			charbuf = chnGetTimeout(&SDU1, 1000);
			if (charbuf != Q_TIMEOUT) {
				if ((char)charbuf == '\r') {
					chprintf( (BaseSequentialStream *)&SDU1, "\r\n", (char)charbuf);	
				} else {
					chprintf( (BaseSequentialStream *)&SDU1, "%c", (char)charbuf);
				}
				if ((char)charbuf == 'e') {
					// Exit on e
					exit = 1;
				}	
			} else {
				chprintf( (BaseSequentialStream *)&SDU1, "\0");
			}
			getIrDistance();
			chThdSleepMilliseconds(300);

			//cm[2] = (int)(2914.0f / (avg[0] / 4.0f + 5.0f))- 1;
			chprintf( (BaseSequentialStream *)&SDU1, "cm[0]: %i ", cm[0]);
			chprintf( (BaseSequentialStream *)&SDU1, "cm[1]: %i ", cm[1]);
			chprintf( (BaseSequentialStream *)&SDU1, "cm[2]: %i \r ", cm[2]);
		}

		// Exited while loop - means connection broken. Turn off LED, sleep then try connect again
		palClearPad(GPIOD, GPIOD_LED4);
		chThdSleepMilliseconds(1000);
	}
	return 0;
}
