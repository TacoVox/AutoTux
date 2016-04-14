/*
 * hardwarePWM.h
 *
 *  Created on: Apr 4, 2016
 *      Author: jerker
 */

#ifndef HARDWAREPWM_H_
#define HARDWAREPWM_H_

#include "autotuxconfig.h"


/*
 * Sets up the PWM pins etc
 */
void hardwareSetupPWM(void);


/*
 * Setter for the values. Specify an output channel ID
 */
void hardwareSetValuesPWM(PWM_OUTPUT_ID, int value);


#endif /* HARDWAREPWM_H_ */
