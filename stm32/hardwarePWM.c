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


#define ENGINE_NEUTRAL 1350
#define ENGINE_FORWARD 1420
#define ENGINE_REV 1140

#define WHEELS_CENTERED 1590
#define WHEELS_MAXRIGHT 1980
#define WHEELS_MAXLEFT 1220


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
	palSetPadMode(GPIOA, 0, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOA, 1, PAL_MODE_ALTERNATE(2));
	pwmStart(&PWMD5, &pwmcfg);
	pwmEnableChannel(&PWMD5, 0, ENGINE_NEUTRAL);
	pwmEnableChannel(&PWMD5, 1, WHEELS_CENTERED);
}



/*
 * Setter for the values. Specify an output channel ID
 */
void hardwareSetValuesPWM(PWM_OUTPUT_ID pwm_id, icucnt_t pw) {
	(void) pwm_id;
	(void) pw;
	if (pwm_id == PWM_OUTPUT_SERVO) {
		switch(pw) {
			case 2:
				pwmEnableChannel(&PWMD5, 1, WHEELS_MAXRIGHT);
				break;
			case 1:
				pwmEnableChannel(&PWMD5, 1, WHEELS_CENTERED);
				break;
			case 0:
				pwmEnableChannel(&PWMD5, 1, WHEELS_MAXLEFT);
				break;
			}
	} else if (pwm_id == PWM_OUTPUT_ESC) {
		switch(pw) {
			case 2:
				pwmEnableChannel(&PWMD5, 0, ENGINE_FORWARD);
				break;
			case 1:
				pwmEnableChannel(&PWMD5, 0, ENGINE_NEUTRAL);
				break;
			case 0:
				pwmEnableChannel(&PWMD5, 0, ENGINE_REV);
				break;
		}
	}
}


//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------

