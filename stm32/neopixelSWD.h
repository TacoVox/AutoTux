/*
 * Software driver for WS2812.
 */

#ifndef LEDDRIVER_H_
#define LEDDRIVER_H_

#include <hal.h>

/**
 * Configuration struct for the neopixel SW driver.
 * It should be possible to run different LED chains
 * on different pins with different configs.
 */
typedef struct {
	/**
	 * Which port the LEDs are connected to.
	 */
	ioportid_t port;

	/**
	 * Which pin the LEDs are connected to.
	 */
	ioportmask_t pin;

	/**
	 * The number of LEDs you want to control.
	 */
	uint32_t numberOfLEDs;

	/**
	 * Makes sure to enforce the reset/latch time while still in
	 * the syslock part of write. Usually not needed if you use Write()
	 * at a reasonable pace.
	 */
	bool enforceLatchTime;

} neopixelConfig;


/** LEDs start from 0 */
#define LED(i) 2^(i - 1)

void neopixelInit(neopixelConfig* cfg, uint8_t** colorBuffer);
void neopixelSetColor(uint8_t* colorBuffer, uint32_t ledMask, unsigned char R, unsigned char G, unsigned char B);
void neopixelWrite(neopixelConfig* cfg, uint8_t* colorBuffer);

#endif /* LEDDRIVER_H_ */
