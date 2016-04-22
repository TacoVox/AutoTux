#include "LEDDriver.h"

// TODO: include choise of pin on init!!!!

#include <hal.h>



#define HIGH_TIME_1 38 // 800 ns // 40
#define LOW_TIME_1 22 // 450 ns // 20
#define HIGH_TIME_0 18 // 400 ns // 15 // 12 för lågt, 24 för högt
#define LOW_TIME_0 43 // 850 ns // 40 - TRY 41
#define RESET_TIME 5 // 50 microseconds
#define RESET_TIME_CLEAR 5


unsigned char *ledColorBuffer;
uint8_t ledCount;

void init(uint8_t newLedCount) {
	palSetPadMode(GPIOA, 6, PAL_MODE_OUTPUT_OPENDRAIN);
	ledCount = newLedCount;
	ledColorBuffer = chHeapAlloc(NULL, sizeof(unsigned char) * newLedCount * 3);
}


void setColor(uint32_t ledMask, unsigned char R, unsigned char G, unsigned char B) {
	for (int i = 0; i < ledCount; i++) {
		if (ledMask & (1 << i)) {
			//GRB =  (G << 16) | (R << 8) | B;
			ledColorBuffer[i * 3] = G;
			ledColorBuffer[i * 3 + 1] = R;
			ledColorBuffer[i * 3 + 2] = B;
		}
	}
}

void write(void) {
	uint32_t color_buffer;
	chSysLock();
	int c = 0;
	for (int currentLED = 0; currentLED < ledCount; currentLED++) {
		//color_buffer = ledColorBuffer[currentLED * 3] << 16 | ledColorBuffer[currentLED * 3 + 1] << 8 |
		//		ledColorBuffer[currentLED * 3 + 2];
		color_buffer = 0x00FF00;
		for (int i = 24; i > 0; i--) {
			// See if bit i is set
			if ((color_buffer & (uint32_t)(0x01 << i - 1)) == (uint32_t)(0x01 << i - 1)) {
				// It's a 1 - 0.8 ms high, 0.45 ms low
				palSetPad(GPIOA, 6);
				for (c = 0; c < HIGH_TIME_1; c++) __asm__("nop");
						// Note probably just the loop consumes significant time
				palClearPad(GPIOA, 6);
				for (c = 0; c < LOW_TIME_1; c++) __asm__("nop");
			} else {
				// It's a 0 - 0.4 ms high, 0.85 ms low
				palSetPad(GPIOA, 6);
				for (c = 0; c < HIGH_TIME_0 ; c++) __asm__("nop"); // Note probably just the loop consumes significant time
				palClearPad(GPIOA, 6);
				for (c = 0; c < LOW_TIME_0 ; c++) __asm__("nop");
			}
		}
		// For some reason, set to HIGH during reset!
		//palSetPad(GPIOA, 6);
		//for (c = 0; c < RESET_TIME; c++) __asm__("nop");
		//palClearPad(GPIOA, 6);
		//for (c = 0; c < RESET_TIME_CLEAR; c++) __asm__("nop");

	}
	chSysUnlock();
}

/////////////////////////////////////
// INTERNAL BELOW
////////////////////////////////////

void writeOneLED(uint32_t color) {


}
