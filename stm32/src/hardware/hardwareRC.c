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


static void icuCallbackThrottle(ICUDriver *icup);
static void icuCallbackSteering(ICUDriver *icup);


static ICUConfig icuConfigThrottle = {
  ICU_INPUT_ACTIVE_HIGH,
  1000000,
  icuCallbackThrottle,
  NULL,
  NULL,
  RC_TIMER_CHANNEL_THROTTLE,
  0
};

static ICUConfig icuConfigSteering = {
  ICU_INPUT_ACTIVE_HIGH,
  1000000,
  icuCallbackSteering,
  NULL,
  NULL,
  RC_TIMER_CHANNEL_STEERING,
  0
};


// The resulting pulsewidth values
static icucnt_t pw[RC_CHANNELS];


//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------


/*
 * Sets up the pins etc.
 */
void hardwareSetupRC() {
	// Throttle
	palSetPadMode(RC_PIN_GROUPS[0], RC_PIN_NUMBERS[0], PAL_MODE_ALTERNATE(2));
	icuStart(RC_TIMER_THROTTLE, &icuConfigThrottle);
	icuStartCapture(RC_TIMER_THROTTLE);
	icuEnableNotifications(RC_TIMER_THROTTLE);

	// Steering
	palSetPadMode(RC_PIN_GROUPS[1], RC_PIN_NUMBERS[1], PAL_MODE_ALTERNATE(2));
	icuStart(RC_TIMER_STEERING, &icuConfigSteering);
	icuStartCapture(RC_TIMER_STEERING);
	icuEnableNotifications(RC_TIMER_STEERING);
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


static void icuCallbackThrottle(ICUDriver *icup) {
	pw[THROTTLE] = icuGetWidthX(icup);
}

static void icuCallbackSteering(ICUDriver *icup) {
	pw[STEERING] = icuGetWidthX(icup);
}
