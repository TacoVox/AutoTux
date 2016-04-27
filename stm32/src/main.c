/** @file	main.c
 *  @brief Main file for the stm32 part of AutoTux.
 *
 * Initializes ChibiOS and the hardware, makes serial connection start it's own
 * thread, and then proceeds to the main loop that reads sensor input and outputs
 * control data to the hardware.
 */


// ChibiOS includes
#include <ch.h>
#include <hal.h>

// Local includes
#include "sensorInput.h"
#include "controlOutput.h"
#include "serial/serialConnection.h"


//-----------------------------------------------------------------------------
// Main loop
//-----------------------------------------------------------------------------


/**
 * @brief Main loop.
 *
 * Initializes ChibiOS and the hardware, makes serial connection start it's own
 * thread, and then proceeds to the main loop that reads sensor input and outputs
 * control data to the hardware.
 */
int main(void) {
	// ChibiOS initialization
	halInit();
	chSysInit();

	// Initialize sensor settings
	sensorInputSetup();
	controlOutputSetup();

	// Start the serial connection. Creates its own thread.
	serialConnectionStart();

	// Then simply read sensor values and output control values on the main thread
	while (true) {
		sensorInputIteration();
		controlOutputIteration();

		// Above meausred to 72 ms including ADC callback - sleep 8 to achieve 12.5 hertz
		chThdSleepMilliseconds(8);
		// Sleep 28 to achieve 10 hz
		// chThdSleepMilliseconds(28);
	}

	return 0;
}
