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

static void updateColorBuffer(void);
static void setLightBools(unsigned char lightByte, bool rcMode, bool rcBrakeLight);
static bool checkBrakeBit(unsigned char lightByte);
static bool checkReverseBit(unsigned char lightByte);
static bool checkFlashLeftBit(unsigned char lightByte);
static bool checkFlashRightBit(unsigned char lightByte);

/**
 * Holds the color buffer for the LED strip
 */
static uint8_t* colorBuffer = 0;

/**
 * Buffer with all 0s to write when lights are turned off
 */
static uint8_t* allLightsOffBuffer = 0;

/**
 * Configuration for the neopixel LED driver
 */
static neopixelConfig cfg = {
	LIGHT_LED_PORT,
	LIGHT_LED_PIN,
	16,
	false
};

/**
 * Master light switch
 */
static bool lightsOn = true;

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
void hardwareLightsSetup(void) {
	palSetPadMode(GPIOC, 7, PAL_MODE_INPUT_PULLUP);
	neopixelInit(&cfg, &colorBuffer);

	// Create all 0s buffer for when lights are turned off
	allLightsOffBuffer = chHeapAlloc(NULL, sizeof(unsigned char) * 16 * 3);
	uint32_t i;
	for (i = 0; i < 16 * 3; i++) allLightsOffBuffer[i] = 0;

	// Headlights
	neopixelSetColor(colorBuffer, LIGHT_HEADLIGHT_LEDS, LIGHT_HEADLIGHT_R, LIGHT_HEADLIGHT_G, LIGHT_HEADLIGHT_B);

	// Tail lights
	neopixelSetColor(colorBuffer, LIGHT_TAILLIGHT_LEDS, LIGHT_TAILLIGHT_R, LIGHT_TAILLIGHT_G, LIGHT_TAILLIGHT_B);

	// Sleep a while to make sure voltage on the car stabilizes before write occurs
	chThdSleepMilliseconds(100);
	lightsHaveChanged = true;
}

/**
 * Determines which lights should be on and off at this point, and forwards to hardware
 * when needed.
 */
void hardwareLightsIteration(unsigned char lightByte, bool rcMode, bool rcBrakeLight) {
	static uint8_t flashStateCounter = 0;

	// First process inputs, refine into bools for which lights are on and off
	setLightBools(lightByte, rcMode, rcBrakeLight);

	// If flashing is turned on, change state regularly.
	if (flashLeft || flashRight) {
		if (flashStateCounter < LIGHT_ITERATIONS_PER_FLASH_STATE - 1) {
			flashStateCounter++;
		} else {
			flashStateCounter = 0;
			flashState = !flashState;
			lightsHaveChanged = true;
		}
	}

	// Check master light switch
	if (lightsOn && palReadPad(GPIOC, 7) == PAL_HIGH) {
		lightsOn = false;
		lightsHaveChanged = true;
	} else if (lightsOn == false && palReadPad(GPIOC, 7) == PAL_LOW) {
		lightsOn = true;
		lightsHaveChanged = true;
	}

	// Rebuild color buffer and write if anything has changed.
	if (lightsHaveChanged) {
		updateColorBuffer();

		if (lightsOn) {
			neopixelWrite(&cfg, colorBuffer);
		} else {
			neopixelWrite(&cfg, allLightsOffBuffer);
		}
		lightsHaveChanged = false;
	}
}

//-----------------------------------------------------------------------------
// Implementation. The static functions below are inaccessible to other modules
//-----------------------------------------------------------------------------

/**
 * Updates the color buffer based on the booleans holding the state of each light.
 */
static void updateColorBuffer(void) {
	// Let flashing activate and deactivate "naturally" aligned with the
	// flashState pace.
	if (flashLeft && flashState) {
		neopixelSetColor(colorBuffer, LIGHT_FLASHLEFT_LEDS, LIGHT_FLASH_R, LIGHT_FLASH_G, LIGHT_FLASH_B);
	}
	if (flashRight && flashState) {
		neopixelSetColor(colorBuffer, LIGHT_FLASHLEFT_LEDS, LIGHT_FLASH_R, LIGHT_FLASH_G, LIGHT_FLASH_B);
	}
	if (!flashState) {
		// Turn off all corner leds
		neopixelSetColor(colorBuffer, LIGHT_FLASHLEFT_LEDS | LIGHT_FLASHRIGHT_LEDS, 0, 0, 0);
	}

	if (reverseLight) {
		neopixelSetColor(colorBuffer, LIGHT_REVERSE_LEDS, LIGHT_REVERSE_R, LIGHT_REVERSE_G, LIGHT_REVERSE_B);
	} else {
		neopixelSetColor(colorBuffer, LIGHT_REVERSE_LEDS, 0, 0, 0);
	}

	if (brakeLight) {
		neopixelSetColor(colorBuffer, LIGHT_BRAKE_LEDS, LIGHT_BRAKE_R, LIGHT_BRAKE_G, LIGHT_BRAKE_B);
	} else {
		neopixelSetColor(colorBuffer, LIGHT_BRAKE_LEDS, 0, 0, 0);
	}

	if (rcLight) {
		neopixelSetColor(colorBuffer, LIGHT_RCLIGHT_LEDS, LIGHT_RCLIGHT_R, LIGHT_RCLIGHT_G, LIGHT_RCLIGHT_B);
	} else {
		neopixelSetColor(colorBuffer, LIGHT_RCLIGHT_LEDS, 0, 0, 0);
	}
}


/**
 * Helper function that chooses which lights should be on and off.
 */
static void setLightBools(unsigned char lightByte, bool rcMode, bool rcBrakeLight) {
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
static bool checkBrakeBit(unsigned char lightByte) {
	return (lightByte &  LIGHT_BIT_BRAKE) == LIGHT_BIT_BRAKE;
}

/**
 * Returns true if the reverse bit is set in the light control byte.
 */
static bool checkReverseBit(unsigned char lightByte) {
	return (lightByte &  LIGHT_BIT_REVERSE) == LIGHT_BIT_REVERSE;
}

/**
 * Returns true if the flash left bit is set in the light control byte.
 */
static bool checkFlashLeftBit(unsigned char lightByte) {
	return (lightByte &  LIGHT_BIT_FLASH_LEFT) == LIGHT_BIT_FLASH_LEFT;
}

/**
 * Returns true if the flash right bit is set in the light control byte.
 */
static bool checkFlashRightBit(unsigned char lightByte) {
	return (lightByte &  LIGHT_BIT_FLASH_RIGHT) == LIGHT_BIT_FLASH_RIGHT;
}
