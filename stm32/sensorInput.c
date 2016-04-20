/*
 * Takes care of sensor input for the stm32 part of AutoTux.
 * Returns a char array of the data that should be passed to the
 * high-level board.
 */


#include <ch.h>
#include <chprintf.h>
#include <hal.h>

#include "sensorInput.h"

// Input
#include "hardwareIR.h"
#include "hardwareUS.h"
#include "hardwareRC.h" // TODO: this should not be here
#include "hardwareWE.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------

// Uncomment for time measurement
//systime_t mstartTime, mendTime;


/**
 * Initialize pins and settings for sensors.
 * Also start the sensor thread.
 */
void sensorInputSetup (void) {
	// Initialize sensors
	hardwareSetupIR();
	hardwareSetupUS();
	hardwareSetupWE();
	hardwareSetupRC();
}


/**
 * Fills a char array with all sensor data
 */
void sensorInputGetData(unsigned char* buffer) {
	// Normal packet output. Allow only values up to 255 to be sure not
	// to get unexpectedly low values caused by conversion/overflow
	buffer[0] = hardwareGetValuesUS(US_FRONT) < 255 ?
		(unsigned char)hardwareGetValuesUS(US_FRONT) : 255;
	buffer[1] = hardwareGetValuesUS(US_SIDE) < 255 ?
		(unsigned char)hardwareGetValuesUS(US_SIDE) : 255;
	buffer[2] = hardwareGetValuesIR(IR_SIDE_FRONT) < 255 ?
		(unsigned char)hardwareGetValuesIR(IR_SIDE_FRONT) : 255;
	buffer[3] = hardwareGetValuesIR(IR_SIDE_REAR) < 255 ?
		(unsigned char)hardwareGetValuesIR(IR_SIDE_REAR) : 255;
	buffer[4] = hardwareGetValuesIR(IR_REAR) ?
		(unsigned char)hardwareGetValuesIR(IR_REAR) : 255;
	buffer[5] = hardwareGetValuesWESpeed() < 255 ?
		(unsigned char)hardwareGetValuesWESpeed() : 255;

	// Distance - int, split into 4 chars
	buffer[6] = hardwareGetValuesWEDistance() >> 24 & 0xFF;
	buffer[7] = hardwareGetValuesWEDistance() >> 16 & 0xFF;
	buffer[8] = hardwareGetValuesWEDistance() >> 8 & 0xFF;
	buffer[9] = hardwareGetValuesWEDistance() & 0xFF;
}


/**
 * Print sensor values to serial
 */
void sensorInputDebugOutput(BaseSequentialStream* SDU) {
	// "\033[F" for going back to previous line
	chprintf(SDU, "\033[FTHROTTLE: %4i ", hardwareGetValuesRC(THROTTLE));
	chprintf(SDU, "STEERING: %4i ", hardwareGetValuesRC(STEERING));
	chprintf(SDU, "WHEEL: %4i ", hardwareGetValuesWESpeed());
	chprintf(SDU, "US FRONT: %3i \r\n", hardwareGetValuesUS(US_FRONT));
	chprintf(SDU, "US SIDE: %3i ", hardwareGetValuesUS(US_SIDE));
	chprintf(SDU, "SIDE_FRONT: %3i ", hardwareGetValuesIR(IR_SIDE_FRONT));
	chprintf(SDU, "SIDE_REAR: %3i ",  hardwareGetValuesIR(IR_SIDE_REAR));
	chprintf(SDU, "REAR: %2i ", hardwareGetValuesIR(IR_REAR));
	chprintf(SDU, "DIST: %4i ", hardwareGetValuesWEDistance());

	// For time measurement

	//chprintf(SDU, "Start: %4i ", ST2MS(mstartTime));
	//chprintf(SDU, "End t: %4i ", ST2MS(mendTime));
	//chprintf(SDU, "End c: %2i ", ST2MS(endTimeCallback));
}



/**
 * Thread iteration
 */
void sensorInputIteration(void) {
	hardwareIterationUSStart();
	hardwareIterationIR();

	chThdSleepMilliseconds(70);
	hardwareIterationUSEnd();
}


//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------

