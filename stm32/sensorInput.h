/*
 * sensorInput.h
 */

#ifndef SENSORINPUT_H_
#define SENSORINPUT_H_


/**
 * Initialize pins and settings for sensors.
 * Also start the sensor thread.
 */
void sensorSetup(void);


/**
 * Fills a char array with all sensor data
 */
void getSensorData(char* buffer);


/**
 * Print sensor values to serial
 */
void sensorDebugOutput(BaseSequentialStream* SDU);


#endif /* SENSORINPUT_H_ */
