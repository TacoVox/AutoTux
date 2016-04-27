/*
 * hardwareWE.h
 *
 *  Created on: Apr 4, 2016
 *      Author: jerker
 */

#ifndef HARDWAREWE_H_
#define HARDWAREWE_H_

#include "../autotuxconfig.h"

/*
 * Sets up the WE sensor pins etc.
 */
void hardwareSetupWE(void);

void hardwareIterationWE(void);


/*
 * Getter for the values. Specify a WE sensor.
 */

//int hardwareGetValuesWE(void);


int hardwareGetValuesWESpeed(void);
int hardwareGetValuesWEDistance(void);

#endif /* HARDWAREWE_H_ */
