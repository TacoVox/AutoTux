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
#include "serialConnection.h"


//-----------------------------------------------------------------------------
// Implementation - main loop.
//-----------------------------------------------------------------------------


int main(void) {
	// ChibiOS initialization
	halInit();
	chSysInit();

	// The main thread is dedicated to the serial connection.
	// In this loop, values are also output to hardware. We will keep this solution
	// unless we have any problems caused by USB output buffer overflow making the
	// writing to USB block, which could cause safety issues with the car if it would
	// ever occur. However it seems to work fine to just avoid writing to the serial
	// connection whenever we perceive the connection as disconnected by reading Q_RESET
	// from the stream.
	serialConnectionLoop();

	return 0;
}
