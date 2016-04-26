/** @file	hardwareLights.c
 * 	@brief Handles the lights of the car.
 *
 * Uses neopixelSWD as the driver for the LEDs.
 */

#include "neopixelSWD.h"
#include "hardwareLights.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

void updateColorBuffer(void);
void setLightBools(unsigned char lightByte, bool rcMode, bool rcBrakeLight);
bool checkBrakeBit(unsigned char lightByte);
bool checkReverseBit(unsigned char lightByte);
bool checkFlashLeftBit(unsigned char lightByte);
bool checkFlashRightBit(unsigned char lightByte);

/**
 * Holds the color buffer for the LED strip
 */
static uint8_t* colorBuffer = 0;

/**
 * Configuration for the neopixel LED driver
 */
static neopixelConfig cfg = {
	LED_PORT,
	LED_PIN,
	16,
	false
};

/**
 * Whether the corresponding lights are currently on or off.
 */
static bool brakeLight, flashLeft, flashRight, reverseLight, rcLight;

/**
 * Used to only write to LED chain if any changes have occured.
 */
static bool lightsHaveChanged;

/**
 * @brief Used to determine if flashing lights are currently in a high or low state.
 *
 * The actual state of whether they are turned on or off from the "driver"
 * perspective is kept in flashLeft and flashRight.
 */
static bool flashState;


//-----------------------------------------------------------------------------
// Public interface
//-----------------------------------------------------------------------------


/*
 * Sets up the pin and color buffer. Initializes head and tail lights.
 */
void hardwareSetupLights(void) {
	neopixelInit(&cfg, &colorBuffer);

	// Headlights
	neopixelSetColor(colorBuffer, HEADLIGHT_LEDS, HEADLIGHT_R, HEADLIGHT_G, HEADLIGHT_B);

	// Tail lights
	neopixelSetColor(colorBuffer, TAILLIGHT_LEDS, TAILLIGHT_R, TAILLIGHT_G, TAILLIGHT_B);

	// Sleep a while to make sure voltage on the car stabilizes before write occurs
	chThdSleepMilliseconds(100);
	lightsHaveChanged = true;
}

/**
 * Determines which lights should be on and off at this point, and forwards to hardware
 * when needed.
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

/**
 * Updates the color buffer based on the booleans holding the state of each light.
 */
void updateColorBuffer(void) {
	// Let flashing activate and deactivate "naturally" aligned with the
	// flashState pace.
	if (flashLeft && flashState) {
		neopixelSetColor(colorBuffer, FLASHLEFT_LEDS, FLASH_R, FLASH_G, FLASH_B);
	}
	if (flashRight && flashState) {
		neopixelSetColor(colorBuffer, FLASHLEFT_LEDS, FLASH_R, FLASH_G, FLASH_B);
	}
	if (!flashState) {
		// Turn off all corner leds
		neopixelSetColor(colorBuffer, FLASHLEFT_LEDS | FLASHRIGHT_LEDS, 0, 0, 0);
	}

	if (reverseLight) {
		neopixelSetColor(colorBuffer, REVERSE_LEDS, REVERSE_R, REVERSE_G, REVERSE_B);
	} else {
		neopixelSetColor(colorBuffer, REVERSE_LEDS, 0, 0, 0);
	}

	if (brakeLight) {
		neopixelSetColor(colorBuffer, BRAKE_LEDS, BRAKE_R, BRAKE_G, BRAKE_B);
	} else {
		neopixelSetColor(colorBuffer, BRAKE_LEDS, 0, 0, 0);
	}

	if (rcLight) {
		neopixelSetColor(colorBuffer, RCLIGHT_LEDS, RCLIGHT_R, RCLIGHT_G, RCLIGHT_B);
	} else {
		neopixelSetColor(colorBuffer, RCLIGHT_LEDS, 0, 0, 0);
	}
}


/**
 * Helper function that chooses which lights should be on and off.
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

/**
 * Returns true if the brake bit is set in the light control byte.
 */
bool checkBrakeBit(unsigned char lightByte) {
	return (lightByte &  LIGHT_BIT_BRAKE) == LIGHT_BIT_BRAKE;
}

/**
 * Returns true if the reverse bit is set in the light control byte.
 */
bool checkReverseBit(unsigned char lightByte) {
	return (lightByte &  LIGHT_BIT_REVERSE) == LIGHT_BIT_REVERSE;
}

/**
 * Returns true if the flash left bit is set in the light control byte.
 */
bool checkFlashLeftBit(unsigned char lightByte) {
	return (lightByte &  LIGHT_BIT_FLASH_LEFT) == LIGHT_BIT_FLASH_LEFT;
}

/**
 * Returns true if the flash right bit is set in the light control byte.
 */
bool checkFlashRightBit(unsigned char lightByte) {
	return (lightByte &  LIGHT_BIT_FLASH_RIGHT) == LIGHT_BIT_FLASH_RIGHT;
}
