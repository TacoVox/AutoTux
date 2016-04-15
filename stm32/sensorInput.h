/*
 * sensorInput.h
 */

#ifndef SENSORINPUT_H_
#define SENSORINPUT_H_


/**
 * Initialize pins and settings for sensors.
 * Also start the sensor thread.
 */
void sensorInputSetup(void);


/**
 * Fills a char array with all sensor data
 */
void sensorInputGetData(unsigned char* buffer);


/**
 * Print sensor values to serial
 */
void sensorInputDebugOutput(BaseSequentialStream* SDU);


#endif /* SENSORINPUT_H_ */
