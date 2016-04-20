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

static THD_WORKING_AREA(sensorThreadWorkingArea, 320); // stack size in bytes
static THD_FUNCTION(sensorThread, arg);
void sensorInputIteration(void);


//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------

// Uncomment for time measurement
// systime_t startTime, endTimeThread, endTimeCallback;


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
	(void)chThdCreateStatic(sensorThreadWorkingArea, sizeof(sensorThreadWorkingArea),
						  NORMALPRIO, sensorThread, NULL);
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
	buffer[5] = hardwareGetValuesWE() < 255 ?
		(unsigned char)hardwareGetValuesWE() : 255;
}


/**
 * Print sensor values to serial
 */
void sensorInputDebugOutput(BaseSequentialStream* SDU) {
	// "\033[F" for going back to previous line
	chprintf(SDU, "\033[FTHROTTLE: %4i ", hardwareGetValuesRC(THROTTLE));
	chprintf(SDU, "STEERING: %4i ", hardwareGetValuesRC(STEERING));
	chprintf(SDU, "WHEEL: %f ", hardwareGetValuesWE());
	chprintf(SDU, "US FRONT: %3i \r\n", hardwareGetValuesUS(US_FRONT));
	chprintf(SDU, "US SIDE: %3i ", hardwareGetValuesUS(US_SIDE));
	chprintf(SDU, "SIDE_FRONT: %3i ", hardwareGetValuesIR(IR_SIDE_FRONT));
	chprintf(SDU, "SIDE_REAR: %3i ",  hardwareGetValuesIR(IR_SIDE_REAR));
	chprintf(SDU, "REAR: %2i ", hardwareGetValuesIR(IR_REAR));

	// For time measurement
	/*
	chprintf(SDU, "Start: %2i ", ST2MS(startTime));
	chprintf(SDU, "End t: %2i ", ST2MS(endTimeThread));
	chprintf(SDU, "End c: %2i ", ST2MS(endTimeCallback));
	*/
}

//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------

static THD_FUNCTION(sensorThread, arg) {
	(void) arg;
	sensorInputIteration();
}


/**
 * Thread iteration
 */
void sensorInputIteration(void) {

	//startTime = chVTGetSystemTime();
	while(true) { // Comment this out to meausre time
		hardwareIterationUSStart();
		hardwareIterationIR();
		//while(hardwareGetValuesIR(IR_REAR) == 0) {
		//	chThdSleepMilliseconds(1);
		//}
		//endTimeCallback = chVTGetSystemTime();

		chThdSleepMilliseconds(70); // TODO: not 65 if less is needed
		hardwareIterationUSEnd();

		// Above meausred to 72 ms including ADC callback - sleep 8 to achieve 12.5 hertz
		chThdSleepMilliseconds(8); // Comment this out to meausre time
	} // Comment this out to meausre time
	//endTimeThread =	chVTGetSystemTime();


}
