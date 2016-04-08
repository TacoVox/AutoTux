/*
 * hardwareIR.h
 *
 *  Created on: Apr 4, 2016
 *      Author: jerker
 */

#ifndef HARDWAREIR_H_
#define HARDWAREIR_H_

#include "autotuxhardware.h"

/*
 * Sets up the IR sensor pins etc.
 */
void hardwareSetupIR(void);

/*
 * Call this each time an analog read should be performed.
 */
void hardwareIterationIR(void);

/*
 * Getter for the values. Specify an IR sensor.
 */
int hardwareGetValuesIR(IR_SENSOR);

#endif /* HARDWAREIR_H_ */