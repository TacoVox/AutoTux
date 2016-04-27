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

// TODO: Consider moving this to the config file!
static void hardwareWEIncrTicks(EXTDriver *extp, expchannel_t channel);
//--
//	Thread Definitions
//--

// TODO: Remove all thread initializations after verifying interrupt routine works.

/*static THD_WORKING_AREA(wheelEncoderThreadWorkingArea, 60); // Stack size in bytes
static THD_FUNCTION(wheelEncoderThread, arg);*/


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

static const EXTConfig extcfg;

//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------

/**
 * Sets up the pin and the thread
 */

// TODO: Refactor this function

void hardwareSetupWE(void) {
	palSetPadMode(WE_PIN_GROUP, WE_PIN_NUMBER, PAL_MODE_INPUT_PULLUP);
	extStart(&EXTD1, &extcfg); /*!< Set up the interrupt */
	startTime = chVTGetSystemTime();

    /*//Set up the thread here
	// TODO: Remove the thread started after verifying interrupt routine works
    (void)chThdCreateStatic(wheelEncoderThreadWorkingArea, sizeof(wheelEncoderThreadWorkingArea),
						  NORMALPRIO, wheelEncoderThread, NULL);*/
}

// TODO: Remove this after verifying interrupt routine works

/*void hardwareIterationWE(void) {
	// NOTE: For now, the calculation won't calculate/affect the speed around systime's reset
	// (every time it's close to overflow)
	// Try measuring every fifth iteration for now



	while(true){
		if (previousEncoderState == FALSE && palReadPad(WE_PIN_GROUP, WE_PIN_NUMBER)) {
	        previousEncoderState = TRUE;
	    } else if (previousEncoderState == TRUE && palReadPad(WE_PIN_GROUP, WE_PIN_NUMBER) == FALSE) {
	        previousEncoderState = FALSE;
	    } // Do we need this guy now?
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

}*/

static void hardwareWEIncrTicks(EXTDriver *extp, expchannel_t channel) {
	(void)extp;
  	(void)channel;

	chSysLockFromISR(); /*!< Starts Kernel Lock Mode */

	distanceTicks++;
	ticks++;
	timeNow = chVTGetSystemTime();

	// Calculation starts here once elapsed time has exceeded 1s

	if (ST2MS(timeNow) > ST2MS(startTime) + 1000) {

		// Do speed calculations here since they depend on time elapsed

		systime_t timeDelta = timeNow - startTime;
		double seconds = ST2MS(timeDelta) / (double)1000;
		double centimeters = ((double)ticks / WE_TICKS_PER_METER) * 100;
		cmPerSecond = (int)(centimeters / seconds);

		// Reset tick counter
		ticks = 0;
		startTime = chVTGetSystemTime();
	}
	chSysUnlockFromISR(); /*!< Ends Kernel Lock Mode */
}

// TODO: Delete me after verifying interrupt routine works

/*static THD_FUNCTION(wheelEncoderThread, arg) {
	(void) arg;
	hardwareIterationWE();
}*/

/**
 * Returns speed in cm/s
 */
int hardwareGetValuesWESpeed(void) {
	return cmPerSecond;
}

/**
 * Returns distance traveled in centimeters
 */

int hardwareGetValuesWEDistance(void) {
	distanceTraveled = ((int)(((float)distanceTicks / (float)WE_TICKS_PER_METER) * 100)+0.5);
	return distanceTraveled;
}

//-----------------------------------------------------------------------------
// Thread Interrupt Config
//-----------------------------------------------------------------------------

//TODO: Consider moving this into config file

static const EXTConfig extcfg = {
  {
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA, hardwareWEIncrTicks},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL}
  }
};
//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------
