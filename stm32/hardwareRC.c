/*
 * hardwareRC.c
 *
 *  Created on: Apr 4, 2016
 *      Author: jerker
 */

#include <hal.h>
#include "hardwareRC.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

static void icuCallback(ICUDriver *icup);

static ICUConfig icuChannel1 = {
  ICU_INPUT_ACTIVE_HIGH,
  1000000,
  icuCallback,
  NULL,
  NULL,
  ICU_CHANNEL_1,
  0
};

static ICUConfig icuChannel2 = {
  ICU_INPUT_ACTIVE_HIGH,
  1000000,
  icuCallback,
  NULL,
  NULL,
  ICU_CHANNEL_2,
  0
};


// The resulting pulsewidth values
icucnt_t pw[RC_CHANNELS];


//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------


/*
 * Sets up the US sensor pins etc.
 */
void hardwareSetupRC() {
	// TODO: MOVE PINS TO CONFIG!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	palSetPadMode(GPIOA, 0, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOA, 1, PAL_MODE_ALTERNATE(2));
}

/*
 * Call this each time an US read should be performed.
 */
void hardwareIterationRC() {

	icuStart(&ICUD5, &icuChannel1);

	icuStartCapture(&ICUD5);
	icuEnableNotifications(&ICUD5);
	chThdSleepMilliseconds(25);
	icuStopCapture(&ICUD5);
	icuStop(&ICUD5);



	icuStart(&ICUD5, &icuChannel2);
	icuStartCapture(&ICUD5);
	icuEnableNotifications(&ICUD5);
	chThdSleepMilliseconds(25);
	icuStopCapture(&ICUD5);
	icuStop(&ICUD5);
}


/*
 * Getter for the values. Specify a US sensor.
 */
icucnt_t hardwareGetValuesRC(RC_CHANNEL channel) {
	return pw[channel];
}

//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------


static void icuCallback(ICUDriver *icup) {
	if (icup->config == &icuChannel1) {
		pw[0] = icuGetWidthX(icup);
	} else {
		pw[1] = icuGetWidthX(icup);
	}
}
