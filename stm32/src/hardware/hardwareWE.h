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

//void hardwareIterationWE(void);


/*
 * Getter for the values. Either speed or distance.
 */

int hardwareGetValuesWESpeed(void); /*!< Returns speed in cm/s */
int hardwareGetValuesWEDistance(void); /*!< Returns distance in cm */

#endif /* HARDWAREWE_H_ */
