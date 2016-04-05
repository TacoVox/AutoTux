/*
 * hardwareUS.h
 *
 *  Created on: Apr 4, 2016
 *      Author: jerker
 */

#ifndef HARDWARERC_H_
#define HARDWARERC_H_

#include "autotuxhardware.h"

/*
 * Sets up the US sensor pins etc.
 */
void hardwareSetupRC(void);

/*
 * Call this each time an US read should be performed.
 */
void hardwareIterationRC(void);

/*
 * Getter for the values. Specify a US sensor.
 */
icucnt_t hardwareGetValuesRC(RC_CHANNEL);

#endif /* HARDWARERC_H_ */
