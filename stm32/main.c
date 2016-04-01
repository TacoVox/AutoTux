/*
 * Main file for the stm32 part of AutoTux
 *
 * Initially based on the USB-CDC example from ChibiOS
 * I imagine having if not classes and polymorphism,
 * at least a similar naming across different sensor handling files
 * for example ir_setup, us_setup etc.
 * Also common functions for invoking the measurement process.
 * To the SETUP methods you also provide a pointer to a container 
		of the result (array if necessary).
 * it will then be transparent to the using code (main loop)
 * if a callback function is used or not.
 *
 */

#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "test.h"
#include "chprintf.h"
#include "usbcfg.h"

typedef enum {SIDE_FRONT, SIDE_REAR, REAR} IR;

int getIrDistance();
void initializeIR();
void adccb(ADCDriver *adcp, adcsample_t *buffer, size_t n);

// Number of channels of ADC
#define ADC_CHANNELS 3

// Sampling amount
#define ADC_SAMPLES 4

static adcsample_t samples[ADC_SAMPLES * ADC_CHANNELS] = {0};
static adcsample_t avg[ADC_CHANNELS];

// 6 = side front, 15 = side rear, 14 = rear
static const ADCConversionGroup adc_group = {
  false,
  ADC_CHANNELS,
  adccb,
  NULL,
  0,
  ADC_CR2_SWSTART,
  ADC_SMPR2_SMP_AN8(ADC_SAMPLE_480) | ADC_SMPR1_SMP_AN14(ADC_SAMPLE_480) | ADC_SMPR1_SMP_AN15(ADC_SAMPLE_480),
  0,
  ADC_SQR1_NUM_CH(ADC_CHANNELS),
  0,
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN8) | ADC_SQR3_SQ2_N(ADC_CHANNEL_IN14) | ADC_SQR3_SQ3_N(ADC_CHANNEL_IN15)  
};

/* works with channel 6, pin A6 - side front
static const ADCConversionGroup adc_group = {
  FALSE,
  ADC_CHANNELS,
  adccb,
  NULL,
  0,
  ADC_CR2_SWSTART,
  ADC_SMPR2_SMP_AN6(ADC_SAMPLE_56),
  0,
  ADC_SQR1_NUM_CH(ADC_CHANNELS),
  0,
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN6) 
};*/




void adccb(ADCDriver *adcp, adcsample_t *buffer, size_t n) {

  (void) buffer; (void) n;
  /* Note, only in the ADC_COMPLETE state because the ADC driver fires an
     intermediate callback when the buffer is half full.*/
	
  	if (adcp->state == ADC_COMPLETE) {
    	/* Calculates the average values from the ADC samples.*/
		avg[0] = (samples[0] + samples[3] + samples[6]) / 3;
		avg[1] = (samples[1] + samples[4] + samples[7]) / 3;
		avg[2] = (samples[2] + samples[5] + samples[8]) / 3;

	}
}









/*
 * Starting point  
 */
int main(void) {
	// Initialize drivers etc
	halInit();
	chSysInit();

	// Initialize IR
	initializeIR();

 	// Initialize serial over USB
	sduObjectInit(&SDU1);
 	sduStart(&SDU1, &serusbcfg);

  	// Activate USB driver, USB pull-up on D+
	// Delay means that if device is reset, it will be unavailable to the
	// host for a while, and then reattached
	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1500);
	usbStart(serusbcfg.usbp, &usbcfg);
  	usbConnectBus(serusbcfg.usbp);

	// Main loop
	while(true) {
		msg_t charbuf;
		int exit = 0; // This can later be switched on timeout!

		while (!exit) {
			// LED on
			palSetPad(GPIOD, GPIOD_LED4);
	
			charbuf = chnGetTimeout(&SDU1, 1000);
			if (charbuf != Q_TIMEOUT) {
				if ((char)charbuf == '\r') {
					chprintf( (BaseSequentialStream *)&SDU1, "\r\n", (char)charbuf);	
				} else {
					chprintf( (BaseSequentialStream *)&SDU1, "%c", (char)charbuf);
				}
				if ((char)charbuf == 'e') {
					// Exit on e
					exit = 1;
				}	
			} else {
				chprintf( (BaseSequentialStream *)&SDU1, "\0");
			}
			getIrDistance();
			chThdSleepMilliseconds(300);
			chprintf( (BaseSequentialStream *)&SDU1, "Avg[0]: %i ", avg[0]);
			chprintf( (BaseSequentialStream *)&SDU1, "Avg[1]: %i ", avg[1]);
			chprintf( (BaseSequentialStream *)&SDU1, "Avg[2]: %i    \r", avg[2]);
	
		}

		// Exited while loop - means connection broken. Turn off LED, sleep then try connect again
		palClearPad(GPIOD, GPIOD_LED4);
		chThdSleepMilliseconds(1000);
	}
	return 0;
}






void initializeIR() {
	// Note: pins are hard coded here!
	adcStart(&ADCD1, NULL);
	palSetPadMode(GPIOB, 0, PAL_MODE_INPUT_ANALOG);
	palSetPadMode(GPIOC, 4, PAL_MODE_INPUT_ANALOG);
	palSetPadMode(GPIOC, 5, PAL_MODE_INPUT_ANALOG);
	
}



int getIrDistance() {
	adcStartConversion(&ADCD1, &adc_group, &samples[0], ADC_SAMPLES);
	return 0;
}
