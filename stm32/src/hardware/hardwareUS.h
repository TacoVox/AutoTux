/** @file	hardwareUS.h
 * 	@brief Reads the US sensor values using i2c. Averages values with a circular buffer.
 */

#ifndef HARDWAREUS_H_
#define HARDWAREUS_H_

#include "../autotuxconfig.h"

/*
 * Sets up the US sensor pins etc.
 */
void hardwareSetupUS(void);

/*
 * @brief Call this each time an US read should be performed.
 *
 * Starts the ranging process. 65 ms later, the range can be read.
 */
void hardwareIterationUSStart(void);

/*
 * @brief Fetches the values from the US sensors.
 *
 * Should be preceeded by a corresponding start call that starts the ranging.
 */
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
