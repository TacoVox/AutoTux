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
static volatile systime_t startTime;

/**
 *	Each tick represents a black stripe measured by the wheel encoder.
 */
uint8_t ticks; //

//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------

/**
 * Sets up the pin and the thread
 */

void hardwareSetupWE(void) {
	palSetPadMode(WE_PIN_GROUP, WE_PIN_NUMBER, PAL_MODE_INPUT_PULLDOWN);
	extStart(&EXTD1, &extcfg); /*!< Set up the interrupt */
	startTime = chVTGetSystemTimeX();
}

static void hardwareWEIncrTicks(EXTDriver *extp, expchannel_t channel) {
	(void)extp;
  	(void)channel;

	distanceTicks++;
	ticks++;

	// Calculation starts here once elapsed time has exceeded 1s
	if (ST2MS(chVTTimeElapsedSinceX(startTime)) > 1000) {

		// Do speed calculations here since they depend on time elapsed

		systime_t timeDelta = chVTTimeElapsedSinceX(startTime);
		double seconds = ST2MS(timeDelta) / (double)1000;
		double centimeters = ((double)ticks / WE_TICKS_PER_METER) * 100;
		cmPerSecond = (int)(centimeters / seconds);

		// Reset speed tick counter
		ticks = 0;
		startTime = chVTGetSystemTimeX();
	}
}

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
