/** @file	hardwareRC.c
 * 	@brief Reads the PWM signals from the RC receiver using ICU timers.
 */

#include <hal.h>
#include "hardwareRC.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


static void icuCallbackThrottle(ICUDriver *icup);
static void icuCallbackSteering(ICUDriver *icup);

/**
 * ICU configuration for the throttle channel.
 */
static ICUConfig icuConfigThrottle = {
  ICU_INPUT_ACTIVE_HIGH,
  1000000,
  icuCallbackThrottle,
  NULL,
  NULL,
  RC_TIMER_CHANNEL_THROTTLE,
  0
};

/**
 * ICU configuration for the steering channel.
 */
static ICUConfig icuConfigSteering = {
  ICU_INPUT_ACTIVE_HIGH,
  1000000,
  icuCallbackSteering,
  NULL,
  NULL,
  RC_TIMER_CHANNEL_STEERING,
  0
};

/**
 * The resulting pulse width values.
 */
static icucnt_t pw[RC_CHANNELS];


//-----------------------------------------------------------------------------
// Public interface
//-----------------------------------------------------------------------------


/*
 * Sets up the pins etc.
 */
void hardwareRCSetup() {
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
 * Getter for the values. Specify an RC channel.
 */
icucnt_t hardwareRCGetValues(RC_CHANNEL channel) {
	return pw[channel];
}


//-----------------------------------------------------------------------------
// Implementation. The static functions below are inaccessible to other modules
//-----------------------------------------------------------------------------


/**
 * Callback function for the throttle measurement
 */
static void icuCallbackThrottle(ICUDriver *icup) {
	pw[THROTTLE] = icuGetWidthX(icup);
}

/**
 * Callback function for the steering measurement
 */
static void icuCallbackSteering(ICUDriver *icup) {
	pw[STEERING] = icuGetWidthX(icup);
}
