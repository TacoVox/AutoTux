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
static void icuCallback(ICUDriver *icup);


static ICUConfig icuConfig = {
  ICU_INPUT_ACTIVE_HIGH,
  //1000000,
  10000,
  icuCallback,
  NULL,
  NULL,
  WE_TIMER_CHANNEL,
  0
};

// The resulting pulsewidth values
uint8_t ticks;
double metersPerSecond;


//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------


/*
 * Sets up the US sensor pins etc.
 */
void hardwareSetupWE(void) {
	palSetPadMode(WE_PIN_GROUP, WE_PIN_NUMBER, PAL_MODE_ALTERNATE(2));
	icuStart(WE_TIMER, &icuConfig);
	icuStartCapture(WE_TIMER);
	icuEnableNotifications(WE_TIMER);
}


void hardwareIterationWE(void) {
	// NOTE: For now, the calculation won't calculate/affect the speed around systime's reset
	// (every time it's close to overflow)
	// Try measuring every fifth iteration for now
	const int n = 5;
	const float ticksPerMeter = 47.7;
	static int counter = 5;
	if (counter < n - 1) {
		counter++;
	} else {
		// Get current time
		systime_t timeNow = chVTGetSystemTime();

		// Calculate time passed since last reset
		if (timeNow > measurementStart) {
			systime_t timeDelta = timeNow - measurementStart;
			int timeDeltaMS = (int)ST2MS(timeDelta);
			double ticksPerS = (ticks * 1000 / (double)timeDeltaMS);
			metersPerSecond = (ticksPerS / ticksPerMeter);
		} else {
			// TODO: could still calculate if we now the max value of systime_t
		}

		// Reset timer and tick counter
		ticks = 0;
		measurementStart = chVTGetSystemTime();
		counter = 0;
	}
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


static void icuCallback(ICUDriver *icup) {
	(void)icup;
	ticks++;
}
