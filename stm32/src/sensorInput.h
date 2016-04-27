/** @file sensorInput.h
 *  @brief Takes care of sensor input for the stm32 part of AutoTux.
 *
 * Runs on the main thread, and serialConnection can use the getter
 * sensorInputGetData that returns a char array of the data that should be passed
 * to the high-level board.
 *
 * This module also has a function for printing debug output, since
 * it's common to want to look at the sensor value readings.
 */


#ifndef SENSORINPUT_H_
#define SENSORINPUT_H_


/**
 * Initialize pins and settings for sensors.
 */
void sensorInputSetup(void);

/**
 * Called each iteration of the main loop to read sensor values.
 */
void sensorInputIteration(void);

/**
 * @brief Fills a char array with all the latest sensor data.
 *
 * serialConnection sends this array to packetHandler which in turn
 * build and sends a sensor data packet.
 */
void sensorInputGetData(unsigned char* buffer);

/**
 * Print sensor values to serial
 */
void sensorInputDebugOutput(BaseSequentialStream* SDU);


#endif /* SENSORINPUT_H_ */
