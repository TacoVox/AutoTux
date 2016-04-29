/** @file	hardwareRC.h
 * 	@brief Reads the PWM signals from the RC receiver using ICU timers.
 */

#ifndef HARDWARERC_H_
#define HARDWARERC_H_

#include "../autotuxconfig.h"


/*
 * Sets up the pins etc.
 */
void hardwareRCSetup(void);

/*
 * Getter for the values. Specify an RC channel.
 */
icucnt_t hardwareRCGetValues(RC_CHANNEL);


#endif /* HARDWARERC_H_ */
