/*
 * Software driver for WS2812.
 */

#ifndef LEDDRIVER_H_
#define LEDDRIVER_H_

#include <hal.h>


// LEDs start from 1!
#define LED(i) 2^i

void init(uint8_t newLedCount);
void setColor(uint32_t ledMask, unsigned char R, unsigned char G, unsigned char B);
void write(void);
void writeOneLED(uint32_t color);
#endif /* LEDDRIVER_H_ */
