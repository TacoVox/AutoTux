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
//static void icuCallback(ICUDriver *icup);

//--
//	Thread Definitions
//--

static THD_WORKING_AREA(wheelEncoderThreadWorkingArea, 150);
static THD_FUNCTION(wheelEncoderThread, arg);

// The resulting pulsewidth values
uint8_t ticks;
bool previousState;
double metersPerSecond;
systime_t startTime;
unsigned int timeNow;


//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------


/*
 * Sets up the US sensor pins etc.
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

	    if (ST2MS(chVTGetSystemTime()) > ST2MS(startTime) + 1000) {
	        // Do calculations here
	        // numberOfTicksInMeters / timeElapsed
	        systime_t timeDelta = timeNow - startTime;
	        int timeDeltaMS = (int)ST2MS(timeDelta); // Why not just initialize this as double?
			double seconds = (double)timeDeltaMS / 1000;
	        double ticksPerS = ticks / seconds; // Why multiply by 1000?????
	        metersPerSecond = (ticksPerS / ticksPerMeter);

	        startTime = chVTGetSystemTime();
	    }
	}
}

static THD_FUNCTION(wheelEncoderThread, arg) {
	(void) arg;
	hardwareIterationWE();
}

/*
 * Getter for the values. Specify a US sensor.
 */
double hardwareGetValuesWE(void) {
	return metersPerSecond;
}

//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------


/*static void icuCallback(ICUDriver *icup) {
	(void)icup;
	ticks++;
}*/
