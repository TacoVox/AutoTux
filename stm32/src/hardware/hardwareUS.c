/** @file	hardwareUS.c
 * 	@brief Reads the US sensor values using i2c. Averages values with a circular buffer.
 */

#include <hal.h>

#include "hardwareUS.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


static int averageWithCircBuffer(int latestValue, US_SENSOR sensor);


#define CMD_REG	0x00		//!< The command register of the US sensors (write)
#define GAIN_REG 0x01		//!< The gain register of the US sensors (write)
#define LIGHT_REG 0x01		//!< The light register of the US sensors (read)
#define RANGE_REG 0x02		//!< The range egister of the US sensors (read)
#define GAIN_SETTING 0x12	//!< Sets maximum analog gain
#define START_RANGING 0x51	//!< The range command of the US sensors


/**
 * Transfer buffer for the i2c communication.
 */
static uint8_t transferBuffer[2];

/**
 * Receive buffer for the i2c communication.
 */
static uint8_t receiveBuffer[2];

/**
 * I2C driver configuration.
 */
static const I2CConfig i2cConfig = {
    OPMODE_I2C,
	 /*400000,
	  FAST_DUTY_CYCLE_2*/
    100000,
	STD_DUTY_CYCLE,
};

/**
 * The resulting centimeter values
 */
static int usCm[US_SENSORS];

/**
 * The resulting light sensor values
 */
static unsigned char lightSensorReading;

/*
 * Circular buffer length
 */
#define US_CIRCULAR_BUFFER_LENGTH 4

/**
 * Circular buffer, new values will be averaged with the others
 */
static int usCmCircBuffer[US_SENSORS][US_CIRCULAR_BUFFER_LENGTH];


//-----------------------------------------------------------------------------
// Public interface
//-----------------------------------------------------------------------------


/*
 * Sets up the US sensor pins etc.
 */
void hardwareUSSetup() {
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
 * @brief Call this each time an US read should be performed.
 *
 * Starts the ranging process. 65 ms later, the range can be read.
 */
void hardwareUSIterationStart() {

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

/*
 * @brief Fetches the values from the US sensors.
 *
 * Should be preceeded by a corresponding start call that starts the ranging.
 */
void hardwareUSIterationEnd() {
	msg_t status = MSG_OK;
	systime_t timeout = MS2ST(10);
	static int zeroOccurences = 0;

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
			if (value == 0 && zeroOccurences <= 2) {
				zeroOccurences++;
			} else {
				// Assign value.
				usCm[currentSensor] = averageWithCircBuffer(value, currentSensor);

			}
			if (value > 0) {
				zeroOccurences = 0;
			}

		} else {
			// Error message.
			//usCm[i] = (int)i2cGetErrors(US_I2C_DRIVER);
		}
	}

	// Also check the light sensor reading on US_FRONT!
	transferBuffer[0] = LIGHT_REG;
	status = i2cMasterTransmitTimeout(US_I2C_DRIVER, US_ADDRESS[US_FRONT], transferBuffer, 1, receiveBuffer, 1, timeout);
	if (status == MSG_OK && receiveBuffer[0] != 0) {
		// Only write non-zero values.
		lightSensorReading = receiveBuffer[0];
	}

	i2cReleaseBus(US_I2C_DRIVER);
	i2cStop(US_I2C_DRIVER);
}

/*
 * Getter for the values. Specify a US sensor.
 */
int hardwareUSGetValues(US_SENSOR sensor) {
	return (usCm[sensor] < US_VALUE_CAP) ? usCm[sensor] : US_VALUE_CAP;
}

/*
 * Getter for the light sensor reading
 */
unsigned char hardwareUSGetLightValue(void) {
	return lightSensorReading;
}


//-----------------------------------------------------------------------------
// Implementation. The static functions below are inaccessible to other modules
//-----------------------------------------------------------------------------


/**
 * Averages the new value with the circular buffer and returns the result.
 */
static int averageWithCircBuffer(int latestValue, US_SENSOR sensor) {
	static int currentPosition[US_SENSORS] = {0};

	// Add to next position in circular buffer
	usCmCircBuffer[sensor][currentPosition[sensor]] = latestValue;
	if (currentPosition[sensor] == US_CIRCULAR_BUFFER_LENGTH -1) {
		currentPosition[sensor] = 0;
	} else {
		currentPosition[sensor]++;
	}

	// Average
	int totalCm = 0;
	for (int i = 0; i < US_CIRCULAR_BUFFER_LENGTH; i++) {
		totalCm += usCmCircBuffer[sensor][i];
	}

	// Round float division and return
	return (int)(((float)totalCm / US_CIRCULAR_BUFFER_LENGTH) + 0.5);
}
