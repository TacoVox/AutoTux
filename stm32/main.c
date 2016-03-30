/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "test.h"

#include "chprintf.h"

#include "usbcfg.h"

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)
#define TEST_WA_SIZE    THD_WORKING_AREA_SIZE(256)



/*
 * Application entry point.
 */
int main(void) {
  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Initializes a serial-over-USB CDC driver.
   */
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  /*
   * Activates the USB driver and then the USB bus pull-up on D+.
   * Note, a delay is inserted in order to not have to disconnect the cable
   * after a reset.
   */
  /*
   * Shell manager initialization.
   */
  //shellInit();



	// Loop this - check in the inner loop if USB driver says still connected,
	// otherwise go back here to reconnect
	usbDisconnectBus(serusbcfg.usbp);
  	chThdSleepMilliseconds(1500);
  	usbStart(serusbcfg.usbp, &usbcfg);
  	usbConnectBus(serusbcfg.usbp);

	msg_t charbuf;
	while(true) {
		charbuf = chnGetTimeout(&SDU1, 1000);
		if (charbuf != Q_TIMEOUT) {
			if ((char)charbuf == '\r') {
				chprintf( (BaseSequentialStream *)&SDU1, "%c", (char)charbuf);				} else {
				chprintf( (BaseSequentialStream *)&SDU1, "%c", (char)charbuf);
			}
			chprintf( (BaseSequentialStream *)&SDU1, "%c", (char)charbuf);
		} else {
			chprintf( (BaseSequentialStream *)&SDU1, "\0");
		}
		chThdSleepMilliseconds(20);
	}
}
