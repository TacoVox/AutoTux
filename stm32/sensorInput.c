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
#include "hardwareRC.h"
//#include "hardwareWE.h"

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
void sensorSetup (void) {
	// Initialize sensors
	hardwareSetupIR();
	hardwareSetupUS();

	(void)chThdCreateStatic(sensorThreadWorkingArea, sizeof(sensorThreadWorkingArea),
						  NORMALPRIO, sensorThread, NULL);
}


/**
 * Fills a char array with all sensor data
 */
void getSensorData(char* buffer) {
	// Normal packet output
	buffer[0] = (char)hardwareGetValuesUS(US_FRONT);
	buffer[1] = (char)hardwareGetValuesUS(US_SIDE);
	buffer[2] = (char)hardwareGetValuesIR(IR_SIDE_FRONT);
	buffer[3] = (char)hardwareGetValuesIR(IR_SIDE_REAR);
	buffer[4] = (char)hardwareGetValuesIR(IR_REAR);
}


/**
 * Print sensor values to serial
 */
void sensorDebugOutput(BaseSequentialStream* SDU) {
	// "\033[F" for going back to previous line
	chprintf(SDU, "\033[FTHROTTLE: %4i ", hardwareGetValuesRC(THROTTLE));
	chprintf(SDU, "STEERING: %4i ", hardwareGetValuesRC(STEERING));
	//chprintf(SDU, "WHEEL: %f ", hardwareGetValuesWE());
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

		// Above meausred to 72 ms including ADC callback - sleep 28 to achieve 10 hertz
		chThdSleepMilliseconds(28); // Comment this out to meausre time
	} // Comment this out to meausre time
	//endTimeThread =	chVTGetSystemTime();


}
