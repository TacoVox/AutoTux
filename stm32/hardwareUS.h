/*
 * hardwareUS.h
 *
 *  Created on: Apr 4, 2016
 *      Author: jerker
 */

#ifndef HARDWAREUS_H_
#define HARDWAREUS_H_

#include "autotuxconfig.h"

/*
 * Sets up the US sensor pins etc.
 */
void hardwareSetupUS(void);

/*
 * Call this each time an US read should be performed.
 * Make sure they are at least 65 ms apart.
 */
void hardwareIterationUSStart(void);
void hardwareIterationUSEnd(void);

/*
 * Getter for the values. Specify a US sensor.
 */
int hardwareGetValuesUS(US_SENSOR);

/*
 * Getter for the light sensor reading
 */
unsigned char hardwareGetValuesUSLight(void);


#endif /* HARDWAREUS_H_ */
