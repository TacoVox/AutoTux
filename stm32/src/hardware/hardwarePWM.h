/** @file	hardwarePWM.h
 * 	@brief Handles the PWM output that controls the servo and ESC.
 */

#ifndef HARDWAREPWM_H_
#define HARDWAREPWM_H_

#include "../autotuxconfig.h"


/*
 * Sets up the PWM pins etc.
 */
void hardwarePWMSetup(void);

/*
 * Setter for the values. Specify an output channel ID.
 */
void hardwarePWMSetValues(PWM_OUTPUT_ID, int value);

/*
 * Setter for the values, pulse widths directly from RC transmitter.
 */
void hardwarePWMSetValuesRC(icucnt_t throttle, icucnt_t steering);


#endif /* HARDWAREPWM_H_ */
