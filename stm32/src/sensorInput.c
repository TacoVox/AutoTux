/** @file sensorInput.c
 *  @brief Takes care of sensor input for the stm32 part of AutoTux.
 *
 * Runs on the main thread, and serialConnection can use the getter
 * sensorInputGetData that returns a char array of the data that should be passed
 * to the high-level board.
 *
 * This module also has a function for printing debug output, since
 * it's common to want to look at the sensor value readings.
 */


// ChibiOS includes
#include <ch.h>
#include <chprintf.h>
#include <hal.h>

// Local includes
#include "sensorInput.h"
#include "hardware/hardwareIR.h"
#include "hardware/hardwareUS.h"
#include "hardware/hardwareWE.h"

// Include RC in debug output
#ifdef DEBUG
#include "hardware/hardwareRC.h"
#endif


//-----------------------------------------------------------------------------
// Public interface
//-----------------------------------------------------------------------------


/**
 * Initialize pins and settings for sensors.
 */
void sensorInputSetup (void) {
	// Initialize sensors
	hardwareIRSetup();
	hardwareUSSetup();
	hardwareSetupWE();
}

/**
 * Called each iteration of the main loop to read sensor values.
 */
void sensorInputIteration(void) {
	// Start ranging on US sensors
	hardwareUSIterationStart();

	// Read IR sensor values
	hardwareIRIteration();

	// Make sure to wait a while, then fetch the ranging values from US sensors
	chThdSleepMilliseconds(70);
	hardwareUSIterationEnd();
}

/**
 * @brief Fills a char array with all the latest sensor data.
 *
 * serialConnection sends this array to packetHandler which in turn
 * build and sends a sensor data packet.
 */
void sensorInputGetData(unsigned char* buffer) {
	// Normal packet output. Allow only values up to 255 to be sure not
	// to get unexpectedly low values caused by conversion/overflow
	buffer[0] = hardwareUSGetValues(US_FRONT) < 255 ?
		(unsigned char)hardwareUSGetValues(US_FRONT) : 255;
	buffer[1] = hardwareUSGetValues(US_SIDE) < 255 ?
		(unsigned char)hardwareUSGetValues(US_SIDE) : 255;
	buffer[2] = hardwareIRGetValues(IR_SIDE_FRONT) < 255 ?
		(unsigned char)hardwareIRGetValues(IR_SIDE_FRONT) : 255;
	buffer[3] = hardwareIRGetValues(IR_SIDE_REAR) < 255 ?
		(unsigned char)hardwareIRGetValues(IR_SIDE_REAR) : 255;
	buffer[4] = hardwareIRGetValues(IR_REAR) ?
		(unsigned char)hardwareIRGetValues(IR_REAR) : 255;
	buffer[5] = hardwareGetValuesWESpeed() < 255 ?
		(unsigned char)hardwareGetValuesWESpeed() : 255;

	// Distance - int, split into 4 chars
	buffer[6] = hardwareGetValuesWEDistance() >> 24 & 0xFF;
	buffer[7] = hardwareGetValuesWEDistance() >> 16 & 0xFF;
	buffer[8] = hardwareGetValuesWEDistance() >> 8 & 0xFF;
	buffer[9] = hardwareGetValuesWEDistance() & 0xFF;

	// Light sensor
	buffer[10] = hardwareUSGetLightValue();
}

/**
 * Print sensor values to serial
 */
void sensorInputDebugOutput(BaseSequentialStream* SDU) {
	// "\033[F" for going back to previous line
	chprintf(SDU, "\033[");

	#ifdef DEBUG
	chprintf(SDU, "FTHROTTLE: %4i ", hardwareRCGetValues(THROTTLE));
	chprintf(SDU, "STEERING: %4i ", hardwareRCGetValues(STEERING));
	#endif

	chprintf(SDU, "WHEEL: %4i ", hardwareGetValuesWESpeed());
	chprintf(SDU, "US FRONT: %3i \r\n", hardwareUSGetValues(US_FRONT));
	chprintf(SDU, "US SIDE: %3i ", hardwareUSGetValues(US_SIDE));
	chprintf(SDU, "SIDE_FRONT: %3i ", hardwareIRGetValues(IR_SIDE_FRONT));
	chprintf(SDU, "SIDE_REAR: %3i ",  hardwareIRGetValues(IR_SIDE_REAR));
	chprintf(SDU, "REAR: %2i ", hardwareIRGetValues(IR_REAR));
	chprintf(SDU, "DIST: %4i ", hardwareGetValuesWEDistance());
	chprintf(SDU, "LIGHT: %3i ", hardwareUSGetLightValue());
}
