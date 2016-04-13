/*
 * hardwareWE.h
 *
 *  Created on: Apr 4, 2016
 *      Author: jerker
 */

#ifndef HARDWAREWE_H_
#define HARDWAREWE_H_

#include "autotuxhardware.h"

/*
 * Sets up the US sensor pins etc.
 */
void hardwareSetupWE(void);

void hardwareIterationWE(void);


/*
 * Getter for the values. Specify a US sensor.
 */
int hardwareGetValuesWE(void);

#endif /* HARDWAREWE_H_ */
