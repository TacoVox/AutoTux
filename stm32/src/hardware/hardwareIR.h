/** @file	hardwareIR.h
 * 	@brief Reads the IR sensors.
 *
 * Uses ADC with several samples for each measurements.
 */

#ifndef HARDWAREIR_H_
#define HARDWAREIR_H_

#include "../autotuxconfig.h"


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
