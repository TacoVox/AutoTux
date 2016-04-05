/*
 * hardwareIR.c
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
	 400000,
	  FAST_DUTY_CYCLE_2/*
    100000,
	STD_DUTY_CYCLE,*/
};


//static i2cflags_t errors = 0;

// Prototype for the adc callback function.
//void adcCallback(ADCDriver *adcp, adcsample_t *buffer, size_t n);

// The resulting centimeter values
int usCm[US_SENSORS];


//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------


/*
 * Sets up the US sensor pins etc.
 */
void hardwareSetupUS() {
	// Front sensor - 7 SDA, 6 SCL
    //palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);
    //palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);
    // Side - 9 SDA
    //palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(4) | PAL_MODE_INPUT_PULLUP | PAL_STM32_OTYPE_OPENDRAIN);
    //palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(4) | PAL_MODE_INPUT_PULLUP | PAL_STM32_OTYPE_OPENDRAIN);


    // Side - C9 SDA - Channel 3 i2c
    palSetPadMode(GPIOC, 9, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);
    palSetPadMode(GPIOA, 8, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);


}

/*
 * Call this each time an US read should be performed.
 */
void hardwareIterationUS() {
	/*
	static uint8_t address = 0x73;

	msg_t status = MSG_OK;
	systime_t timeout = MS2ST(100);

	transferBuffer[0] = CMD_REG;
	transferBuffer[1] = START_RANGING;

	i2cStart(&I2CD3, &i2cConfig);

	i2cAcquireBus(&I2CD3);
	// Transmit CMD and START RANGING
	status = i2cMasterTransmitTimeout(&I2CD3, address, transferBuffer, 2, receiveBuffer, 0, timeout);
	i2cReleaseBus(&I2CD3);

	// Wait a while
	chThdSleepMilliseconds(300);

	// Transmit RANGE REG and get reply
	i2cAcquireBus(&I2CD3);
	transferBuffer[0] = RANGE_REG;
	status = i2cMasterTransmitTimeout(&I2CD3, address, transferBuffer, 1, receiveBuffer, 2, timeout);
	i2cReleaseBus(&I2CD3);*

	// Check software rev
	i2cAcquireBus(&I2CD3);
	transferBuffer[0] = 0x00;
	// Transmit CMD and START RANGING
	status = i2cMasterTransmitTimeout(&I2CD3, address, transferBuffer, 1, receiveBuffer, 1, timeout);
	i2cReleaseBus(&I2CD3);


	i2cStop(&I2CD3);

	if (status == MSG_OK){
		usCm[0] = (receiveBuffer[0] << 8) + receiveBuffer[1] receiveBuffer[0];
		usCm[1] = 0x00;
		usCm[2] = 0x00;
	} else {// OK - output address when found
		//usCm[0] = 0x00;
		//usCm[0] =
		usCm[1] = (int)i2cGetErrors(&I2CD3) + 2;
		usCm[2] = address;
	}*/


	for (int i = 0; i < US_SENSORS; i++) {
		msg_t status = MSG_OK;

		systime_t timeout = MS2ST(10);


		transferBuffer[0] = CMD_REG;
		transferBuffer[1] = START_RANGING;


		i2cStart(&I2CD3, &i2cConfig);
		i2cAcquireBus(&I2CD3);
		// Transmit CMD and START RANGING
		status = i2cMasterTransmitTimeout(&I2CD3, US_ADDRESS[i], transferBuffer, 2, receiveBuffer, 0, timeout);
		i2cReleaseBus(&I2CD3);

		// Wait a while
		chThdSleepMilliseconds(70);

		// Transmit RANGE REG and get reply
		i2cAcquireBus(&I2CD3);
		transferBuffer[0] = RANGE_REG;
		status = i2cMasterTransmitTimeout(&I2CD3, US_ADDRESS[i], transferBuffer, 1, receiveBuffer, 2, timeout);
		i2cReleaseBus(&I2CD3);


		i2cStop(&I2CD3);

		if (status != MSG_OK){
			usCm[i] = (int)i2cGetErrors(&I2CD3);
		} else {
			usCm[i] = (receiveBuffer[0] << 8) + receiveBuffer[1];
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

