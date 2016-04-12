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


#define CMD_REG			0x00	// Command register
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
}




/*
 * Call this each time an US read should be performed.
 * Starts the ranging process. 65 ms later, the range can be read.
 */
void hardwareIterationUSStart() {
	for (int i = 0; i < US_SENSORS; i++) {
		systime_t timeout = MS2ST(10);

		transferBuffer[0] = CMD_REG;
		transferBuffer[1] = START_RANGING;

		// Transmit CMD and START RANGING
		i2cStart(US_I2C_DRIVER, &i2cConfig);
		i2cAcquireBus(US_I2C_DRIVER);
		i2cMasterTransmitTimeout(US_I2C_DRIVER, US_ADDRESS[i],
				transferBuffer, 2, receiveBuffer, 0, timeout);
		i2cReleaseBus(US_I2C_DRIVER);

		i2cStop(US_I2C_DRIVER);
	}
}


void hardwareIterationUSEnd() {
	for (int i = 0; i < US_SENSORS; i++) {
		msg_t status = MSG_OK;
		systime_t timeout = MS2ST(10);

		i2cStart(US_I2C_DRIVER, &i2cConfig);

		// Transmit RANGE REG and get reply
		i2cAcquireBus(US_I2C_DRIVER);
		transferBuffer[0] = RANGE_REG;
		status = i2cMasterTransmitTimeout(US_I2C_DRIVER, US_ADDRESS[i], transferBuffer, 1, receiveBuffer, 2, timeout);
		i2cReleaseBus(US_I2C_DRIVER);

		i2cStop(US_I2C_DRIVER);

		// Store the value
		if (status == MSG_OK) {
			usCm[i] = (receiveBuffer[0] << 8) + receiveBuffer[1];
		} else {
			// Error message.
			//usCm[i] = (int)i2cGetErrors(US_I2C_DRIVER);
		}
	}
}

/*
 * Getter for the values. Specify a US sensor.
 */
int hardwareGetValuesUS(US_SENSOR sensor) {
	return usCm[sensor];
}

//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------

