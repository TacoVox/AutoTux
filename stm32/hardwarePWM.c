/*
 * hardwarePWM.c
 *
 *  Created on: Apr 4, 2016
 *      Author: jerker
 */

#include <hal.h>
#include "hardwareRC.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


int map(int x, int in_min, int in_max, int out_min, int out_max);

static PWMConfig pwmcfg = {
	1000000, // 1Mhz freq
    20000,   // 20 ms period
	NULL,
	{ // Move this to config
			{PWM_OUTPUT_ACTIVE_HIGH, NULL}, // Channel 1 and 2 active
			{PWM_OUTPUT_ACTIVE_HIGH, NULL},
			{PWM_OUTPUT_DISABLED, NULL},
			{PWM_OUTPUT_DISABLED, NULL}
	},
	0,
	0
};


//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------


/*
 * Sets up the pins etc.
 */
void hardwareSetupPWM(void) {
	// TODO: Move pins and timer driver to config!
	palSetPadMode(GPIOA, 0, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOA, 1, PAL_MODE_ALTERNATE(2));
	pwmStart(&PWMD5, &pwmcfg);
	pwmEnableChannel(&PWMD5, 0, SPEED_PULSEWIDTHS[SPEED_STOP]);
	pwmEnableChannel(&PWMD5, 1, WHEELS_CENTERED_PW);
}



/*
 * Setter for the values. Specify an output channel ID
 */
void hardwareSetValuesPWM(PWM_OUTPUT_ID pwm_id, int value) {
	if (pwm_id == PWM_OUTPUT_SERVO) {
		// Map angle linearly to pulsewidth. Different mappings on either side,
		// based on the pulsewidths we perceived as producing the max steering
		// angles.
		if (value < 90) {
			pwmEnableChannel(&PWMD5, 1, map(value, WHEELS_MAXLEFT_ANGLE,
					WHEELS_CENTERED_ANGLE, WHEELS_MAXLEFT_PW, WHEELS_CENTERED_PW));
		} else if (value > 90) {
			pwmEnableChannel(&PWMD5, 1, map(value, WHEELS_CENTERED_ANGLE,
					WHEELS_MAXRIGHT_ANGLE, WHEELS_CENTERED_PW, WHEELS_MAXRIGHT_PW));
		} else {
			// Center
			pwmEnableChannel(&PWMD5, 1, WHEELS_CENTERED_PW);
		}
	} else if (pwm_id == PWM_OUTPUT_ESC) {
		// Set ESC pw to what is stored at SPEED_PULSEWIDTHS[value] provided that
		// "value" is in the valid range of speed steps
		if (value >= 0 && value <= SPEED_STEPS - 1) {
			pwmEnableChannel(&PWMD5, 0, SPEED_PULSEWIDTHS[value]);
		}
	}
}


//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------

/**
 * Map function, borrowed from the Arduino reference manual!
 * Adapted to not allow out of bound values.
 */
int map(int x, int in_min, int in_max, int out_min, int out_max) {
	if (x < in_min) x = in_min;
	if (x > in_max) x = in_max;
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

