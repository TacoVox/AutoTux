/*
 * hardwareRC.h
 *
 *  Created on: Apr 4, 2016
 *      Author: jerker
 */

#ifndef HARDWARERC_H_
#define HARDWARERC_H_

#include "autotuxconfig.h"

/*
 * Sets up the RC sensor pins etc.
 */
void hardwareSetupRC(void);


/*
 * Getter for the values. Specify an RC channel.
 */
icucnt_t hardwareGetValuesRC(RC_CHANNEL);

#endif /* HARDWARERC_H_ */
