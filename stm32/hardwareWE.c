/*
 * hardwareW.c
 *
 *  Created on: Apr 4, 2016
 *      Author: jerker
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

// The resulting pulsewidth values
uint8_t ticks;
bool previousState;
int cmPerSecond;
systime_t startTime;
systime_t timeNow;


//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------


/*
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
    const float ticksPerMeter = 47.7;

	while(true){
		if (previousState == FALSE && palReadPad(WE_PIN_GROUP, WE_PIN_NUMBER)) {
	        previousState = TRUE;
	        ticks++;
	    } else if (previousState == TRUE && palReadPad(WE_PIN_GROUP, WE_PIN_NUMBER) == FALSE) {
	        previousState = FALSE;
	    }
		timeNow = chVTGetSystemTime();
	    if (ST2MS(timeNow) > ST2MS(startTime) + 1000) {
	        // Do calculations here
	        // numberOfTicksInMeters / timeElapsed
	        systime_t timeDelta = timeNow - startTime;
			double seconds = ST2MS(timeDelta) / (double)1000;
			double centimeters = (ticks / ticksPerMeter) * 100;
	        cmPerSecond = (int)(centimeters / seconds);

	        // Reset tick counter
	        ticks = 0;
	        startTime = chVTGetSystemTime();
	    }

		chThdSleepMilliseconds(5);
	}
}

static THD_FUNCTION(wheelEncoderThread, arg) {
	(void) arg;
	hardwareIterationWE();
}

/*
 * Getter for the values. Specify a US sensor.
 */
int hardwareGetValuesWE(void) {
	return cmPerSecond;
}

//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------


