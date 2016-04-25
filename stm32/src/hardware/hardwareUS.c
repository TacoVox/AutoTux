/*
 * hardwareUS.c
 *
 *  Created on: Apr 4, 2016
 *      Author: jerker
 */

#include <hal.h>

#include "hardwareUS.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


int averageWithCircBuffer(int latestValue, US_SENSOR sensor);

#define CMD_REG			0x00	// Command register
#define GAIN_REG		0x01
#define LIGHT_REG		0x01
#define GAIN_SETTING	0x16	// Sets maximum analog gain to 265
#define START_RANGING	0x51
#define RANGE_REG    	0x02

static uint8_t transferBuffer[2];
static uint8_t receiveBuffer[2];

static const I2CConfig i2cConfig = {
    OPMODE_I2C,
	 /*400000,
	  FAST_DUTY_CYCLE_2*/
    100000,
	STD_DUTY_CYCLE,
};


// The resulting centimeter values
int usCm[US_SENSORS];
unsigned char lightSensorReading;

// Circular buffer, values will be averaged with the others
#define US_CIRCULAR_BUFFER_LENGTH 5
int usCmCircBuffer[US_SENSORS][US_CIRCULAR_BUFFER_LENGTH];





//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------


/*
 * Sets up the US sensor pins etc.
 */
void hardwareSetupUS() {
    palSetPadMode(US_PIN_GROUPS[0], US_PIN_NUMBERS[0],
    		PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);
    palSetPadMode(US_PIN_GROUPS[1], US_PIN_NUMBERS[1],
    		PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);


    // Configure the max gain of the sensors
    systime_t timeout = MS2ST(10);

	transferBuffer[0] = GAIN_REG;
	transferBuffer[1] = GAIN_SETTING;

	// Transmit CMD and START RANGING
	i2cStart(US_I2C_DRIVER, &i2cConfig);
	i2cAcquireBus(US_I2C_DRIVER);
	for (int i = 0; i < US_SENSORS; i++) {
		i2cMasterTransmitTimeout(US_I2C_DRIVER, US_ADDRESS[i],
				transferBuffer, 2, receiveBuffer, 0, timeout);
	}
	i2cReleaseBus(US_I2C_DRIVER);
	i2cStop(US_I2C_DRIVER);
}




/*
 * Call this each time an US read should be performed.
 * Starts the ranging process. 65 ms later, the range can be read.
 */
void hardwareIterationUSStart() {

		systime_t timeout = MS2ST(10);

		transferBuffer[0] = CMD_REG;
		transferBuffer[1] = START_RANGING;

		// Transmit CMD and START RANGING
		i2cStart(US_I2C_DRIVER, &i2cConfig);
		i2cAcquireBus(US_I2C_DRIVER);
		for (int i = 0; i < US_SENSORS; i++) {
			i2cMasterTransmitTimeout(US_I2C_DRIVER, US_ADDRESS[i],
					transferBuffer, 2, receiveBuffer, 0, timeout);
		}
		i2cReleaseBus(US_I2C_DRIVER);
		i2cStop(US_I2C_DRIVER);

}


void hardwareIterationUSEnd() {
	msg_t status = MSG_OK;
	systime_t timeout = MS2ST(10);
	static int zero_occurences = 0;

	i2cStart(US_I2C_DRIVER, &i2cConfig);
	i2cAcquireBus(US_I2C_DRIVER);
	for (int currentSensor = 0; currentSensor < US_SENSORS; currentSensor++) {

		// Transmit RANGE REG and get reply
		transferBuffer[0] = RANGE_REG;
		status = i2cMasterTransmitTimeout(US_I2C_DRIVER, US_ADDRESS[currentSensor], transferBuffer, 1, receiveBuffer, 2, timeout);

		// Store the value
		if (status == MSG_OK) {
			int value = (receiveBuffer[0] << 8) + receiveBuffer[1];

			// To smooth out occasional ranging errors,
			// Only update the value to 0 if it has been so for three consecutive times
			if (value == 0 && zero_occurences <= 2) {
				zero_occurences++;
			} else {
				// Assign value.
				usCm[currentSensor] = averageWithCircBuffer(value, currentSensor);

			}
			if (value > 0) {
				zero_occurences = 0;
			}

		} else {
			// Error message.
			//usCm[i] = (int)i2cGetErrors(US_I2C_DRIVER);
		}
	}

	// Also check the light sensor reading on US_FRONT!
	transferBuffer[0] = LIGHT_REG;
	status = i2cMasterTransmitTimeout(US_I2C_DRIVER, US_ADDRESS[US_FRONT], transferBuffer, 1, receiveBuffer, 1, timeout);
	if (status == MSG_OK) {
		lightSensorReading = receiveBuffer[0];
	}

	i2cReleaseBus(US_I2C_DRIVER);
	i2cStop(US_I2C_DRIVER);
}

/*
 * Getter for the values. Specify a US sensor.
 */
int hardwareGetValuesUS(US_SENSOR sensor) {
	return (usCm[sensor] < US_VALUE_CAP) ? usCm[sensor] : US_VALUE_CAP;
}


/*
 * Getter for the light sensor reading
 */
unsigned char hardwareGetValuesUSLight(void) {
	return lightSensorReading;
}

//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------


int averageWithCircBuffer(int latestValue, US_SENSOR sensor) {
	static int currentPosition = 0;

	// Add to next position in circular buffer
	usCmCircBuffer[sensor][currentPosition] = latestValue;
	if (currentPosition == US_CIRCULAR_BUFFER_LENGTH -1) {
		currentPosition = 0;
	} else {
		currentPosition++;
	}

	// Average
	int totalCm = 0;
	for (int i = 0; i < US_CIRCULAR_BUFFER_LENGTH; i++) {
		totalCm += usCmCircBuffer[sensor][i];
	}

	// Round float division and return
	return (int)(((float)totalCm / US_CIRCULAR_BUFFER_LENGTH) + 0.5);
}
