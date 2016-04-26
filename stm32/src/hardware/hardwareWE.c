/** @file	hardwareWE.c
 * 	@brief Measurements and calculations using the Wheel Encoder.
 *
 *	Created on the 4th of April.\n
 *  Includes speed measurements and distance calculations.
 */

#include <hal.h>
#include <ch.h>
#include "hardwareWE.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

systime_t measurementStart;

//--
//	Thread Definitions
//--

static THD_WORKING_AREA(wheelEncoderThreadWorkingArea, 60); // Stack size in bytes
static THD_FUNCTION(wheelEncoderThread, arg);


/**
 *	Each tick represents a black stripe measured by the wheel encoder.
 */
uint8_t ticks;

/**
 *	Ticks used to measure distance traveled
 */
unsigned int distanceTicks;
/**
 *	Used in distance calculations
 */
unsigned int distanceTraveled;
/**
	TRUE if the wheel encoder state has changed (i.e. passed from a black strip to wheel)
	FALSE otherwise. A FALSE state will ıncrement ticks and distanceTicks.
*/
bool previousEncoderState;
/**
	centimeters traveled / seconds passed.
	Calculated in hardwareIterationWE using ticks as well as timeDelta.
*/

int cmPerSecond;
/**
	Systime at boot and is reset approx every 1000ms.
*/

systime_t startTime;
systime_t timeNow;




//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------


/**
 * Sets up the pin and the thread
 */
void hardwareSetupWE(void) {
	palSetPadMode(WE_PIN_GROUP, WE_PIN_NUMBER, PAL_MODE_INPUT_PULLUP);
    //Set up the thread here
    (void)chThdCreateStatic(wheelEncoderThreadWorkingArea, sizeof(wheelEncoderThreadWorkingArea),
						  NORMALPRIO, wheelEncoderThread, NULL);
}

void hardwareIterationWE(void) {
	// NOTE: For now, the calculation won't calculate/affect the speed around systime's reset
	// (every time it's close to overflow)
	// Try measuring every fifth iteration for now

	systime_t startTime = chVTGetSystemTime();

	while(true){
		if (previousEncoderState == FALSE && palReadPad(WE_PIN_GROUP, WE_PIN_NUMBER)) {
	        previousEncoderState = TRUE;
	        ticks++;
			distanceTicks++;
	    } else if (previousEncoderState == TRUE && palReadPad(WE_PIN_GROUP, WE_PIN_NUMBER) == FALSE) {
	        previousEncoderState = FALSE;
	    }
		timeNow = chVTGetSystemTime();
	    if (ST2MS(timeNow) > ST2MS(startTime) + 1000) {
	        // Do calculations here
	        // numberOfTicksInMeters / timeElapsed
	        systime_t timeDelta = timeNow - startTime;
			double seconds = ST2MS(timeDelta) / (double)1000;
			double centimeters = ((double)ticks / WE_TICKS_PER_METER) * 100;
	        cmPerSecond = (int)(centimeters / seconds);

	        // Reset tick counter
	        ticks = 0;
	        startTime = chVTGetSystemTime();
	    }

		chThdSleepMilliseconds(1);
	}
}

static THD_FUNCTION(wheelEncoderThread, arg) {
	(void) arg;
	hardwareIterationWE();
}

/*
 * Getter for the values. Specify a US sensor.
 */
int hardwareGetValuesWESpeed(void) {
	return cmPerSecond;
}

/*
 * Returns distance traveled in centimeters
 */

int hardwareGetValuesWEDistance(void) {
	distanceTraveled = ((int)(((float)distanceTicks / (float)WE_TICKS_PER_METER) * 100)+0.5);
	return distanceTraveled;
}

//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------
