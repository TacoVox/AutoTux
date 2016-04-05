/*
 * hardwareUS.h
 *
 *  Created on: Apr 4, 2016
 *      Author: jerker
 */

#ifndef HARDWAREUS_H_
#define HARDWAREUS_H_

#include "autotuxhardware.h"

/*
 * Sets up the US sensor pins etc.
 */
void hardwareSetupUS(void);

/*
 * Call this each time an US read should be performed.
 */
void hardwareIterationUS(void);

/*
 * Getter for the values. Specify a US sensor.
 */
int hardwareGetValuesUS(US_SENSOR);

#endif /* HARDWAREUS_H_ */
