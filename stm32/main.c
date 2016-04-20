/**
 * Main file for the stm32 part of AutoTux.
 * Some of the files such as usbcfg.*, chconf, mcuconf and halconf are from the ChibiOS
 * examples for stm32. The config files are tailored for our particular needs.
 * Also note this from the ChibiOS example readme:
 *
 * "Some files used by the demo are not part of ChibiOS/RT but are copyright of
 * ST Microelectronics and are licensed under a different license.
 * Also note that not all the files present in the ST library are distributed
 * with ChibiOS/RT, you can find the whole library on the ST web site."
 */


// ChibiOS includes
#include <ch.h>
#include <hal.h>

// Local includes
#include "sensorInput.h"
#include "controlOutput.h"
#include "serialConnection.h"


//-----------------------------------------------------------------------------
// Implementation - main loop.
//-----------------------------------------------------------------------------


int main(void) {
	// ChibiOS initialization
	halInit();
	chSysInit();

	// Initialize sensor settings
	sensorInputSetup();
	controlOutputSetup();

	// Start another thread for the serial connection
	serialConnectionStart();

	// Then simply read sensor values and output control values on the main thread
	while (true) {
		sensorInputIteration();
		controlOutputIteration();

		// Above meausred to 72 ms including ADC callback - sleep 8 to achieve 12.5 hertz
		chThdSleepMilliseconds(8); // Comment this out to meausre time
	}

	return 0;
}
