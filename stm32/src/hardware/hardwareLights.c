/*
 * hardwareLights.c
 */

#include "neopixelSWD.h"
#include "hardwareLights.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define ITERATIONS_PER_FLASH_STATE 4

void updateColorBuffer(void);
void setLightBools(unsigned char lightByte, bool rcMode, bool rcBrakeLight);
bool checkBrakeBit(unsigned char lightByte);
bool checkReverseBit(unsigned char lightByte);
bool checkFlashLeftBit(unsigned char lightByte);
bool checkFlashRightBit(unsigned char lightByte);

static uint8_t* colorBuffer = 0;
static neopixelConfig cfg = {
	GPIOA, // TODO: move pin to config
	6,
	16,
	false
};
static bool brakeLight, flashLeft, flashRight, reverseLight, rcLight;
static bool lightsHaveChanged, flashState;

//-----------------------------------------------------------------------------
// Public interface
//-----------------------------------------------------------------------------


/*
 * Sets up the pin and color buffer.
 */
void hardwareSetupLights(void) {

	neopixelInit(&cfg, &colorBuffer);

	// Headlights
	neopixelSetColor(colorBuffer, LED(1) | LED(2) | LED(5) | LED(6), 50, 50, 50);

	// Tail lights
	neopixelSetColor(colorBuffer, LED(9) | LED(14), 40, 0, 0);

	// Sleep a while to make sure voltage on the car stabilizes before write occurs
	chThdSleepMilliseconds(100);
	lightsHaveChanged = true;
}

/**
 *
 */
void hardwareIterationLights(unsigned char lightByte, bool rcMode, bool rcBrakeLight) {
	static uint8_t flashStateCounter = 0;

	// First process inputs, refine into bools for which lights are on and off
	setLightBools(lightByte, rcMode, rcBrakeLight);

	// If flashing is turned on, change state regularly.
	if (flashLeft || flashRight) {
		if (flashStateCounter < ITERATIONS_PER_FLASH_STATE - 1) {
			flashStateCounter++;
		} else {
			flashStateCounter = 0;
			flashState = !flashState;
			lightsHaveChanged = true;
		}
	}

	// Rebuild color buffer and write if anything has changed.
	if (lightsHaveChanged) {
		updateColorBuffer();

		neopixelWrite(&cfg, colorBuffer);
		lightsHaveChanged = false;
	}
}

//-----------------------------------------------------------------------------
// Implementation. The static functions below are inaccessible to other modules
//-----------------------------------------------------------------------------

void updateColorBuffer(void) {
	// Let flashing activate and deactivate "naturally" aligned with the
	// flashState pace.
	if (flashLeft && flashState) {
		neopixelSetColor(colorBuffer, LED(7) | LED(8), 230, 130, 0);
	}
	if (flashRight && flashState) {
		neopixelSetColor(colorBuffer, LED(0) | LED(15), 230, 130, 0);
	}
	if (!flashState) {
		// Turn off all corner leds
		neopixelSetColor(colorBuffer, LED(0) | LED(7) | LED(8) | LED(15), 0, 0, 0);

	}

	if (reverseLight) {
		neopixelSetColor(colorBuffer, LED(11), 60, 60, 60);
	} else {
		neopixelSetColor(colorBuffer, LED(11), 0, 0, 0);
	}

	if (brakeLight) {
		neopixelSetColor(colorBuffer, LED(10) | LED(13), 255, 0, 0);
	} else {
		neopixelSetColor(colorBuffer, LED(10) | LED(13), 0, 0, 0);
	}

	if (rcLight) {
		neopixelSetColor(colorBuffer, LED(3) | LED(4) | LED(12), 0, 0, 150);
	} else {
		neopixelSetColor(colorBuffer, LED(3) | LED(4) | LED(12), 0, 0, 0);
	}
}


/**
 * Choose which lights should be on and off
 */
void setLightBools(unsigned char lightByte, bool rcMode, bool rcBrakeLight) {
	// Assumption: rcLight = (rcMode || rcBrakeLight)
	if (rcLight != (rcMode || rcBrakeLight)) {
		rcLight = (rcMode || rcBrakeLight);
		lightsHaveChanged = true;
	}

	// Assumption: brakeLight = (rcBrakeLight || brake bit set in lightByte)
	if (brakeLight != (rcBrakeLight || checkBrakeBit(lightByte))) {
		brakeLight = (rcBrakeLight || checkBrakeBit(lightByte));
		lightsHaveChanged = true;
	}

	// Similar below
	if (reverseLight != checkReverseBit(lightByte)) {
		reverseLight = checkReverseBit(lightByte);
		lightsHaveChanged = true;
	}
	if (flashLeft != checkFlashLeftBit(lightByte)) {
		flashLeft = checkFlashLeftBit(lightByte);
		lightsHaveChanged = true;
	}
	if (flashRight != checkFlashRightBit(lightByte)) {
		flashRight = checkFlashRightBit(lightByte);
		lightsHaveChanged = true;
	}
}

bool checkBrakeBit(unsigned char lightByte) {
	return (lightByte &  LIGHT_BIT_BRAKE) == LIGHT_BIT_BRAKE;
}

bool checkReverseBit(unsigned char lightByte) {
	return (lightByte &  LIGHT_BIT_REVERSE) == LIGHT_BIT_REVERSE;
}

bool checkFlashLeftBit(unsigned char lightByte) {
	return (lightByte &  LIGHT_BIT_FLASH_LEFT) == LIGHT_BIT_FLASH_LEFT;
}

bool checkFlashRightBit(unsigned char lightByte) {
	return (lightByte &  LIGHT_BIT_FLASH_RIGHT) == LIGHT_BIT_FLASH_RIGHT;
}
