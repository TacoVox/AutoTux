/** @file	hardwareLights.h
 * 	@brief Handles the lights of the car.
 *
 * Uses neopixelSWD as the driver for the LEDs.
 */

#ifndef HARDWARELIGHTS_H_
#define HARDWARELIGHTS_H_

#include "../autotuxconfig.h"


/**
 * The brake bit of the light control byte.
 */
#define LIGHT_BIT_BRAKE (1 << 0)

/**
 * The reverse bit of the light control byte.
 */
#define LIGHT_BIT_REVERSE (1 << 1)

/**
 * The flash left bit of the light control byte.
 */
#define LIGHT_BIT_FLASH_LEFT (1 << 2)

/**
 * The flash right bit of the light control byte.
 */
#define LIGHT_BIT_FLASH_RIGHT (1 << 3)


/*
 * Sets up the pin and color buffer. Initializes head and tail lights.
 */
void hardwareLightsSetup(void);

/**
 * Determines which lights should be on and off at this point, and forwards to hardware
 * when needed.
 */
void hardwareLightsIteration(unsigned char lightByte, bool rcMode, bool rcBrakeLight);


#endif /* HARDWARELIGHTS_H_ */
