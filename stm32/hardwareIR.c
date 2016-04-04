/*
 * hardwareIR.c
 *
 *  Created on: Apr 4, 2016
 *      Author: jerker
 */

#include "hardwareIR.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


// Prototype for the adc callback function.
void adcCallback(ADCDriver *adcp, adcsample_t *buffer, size_t n);

// Sample buffer array + array for averages
adcsample_t irSamples[ADC_SAMPLES * ADC_CHANNELS] = {0};
adcsample_t irAvg[ADC_CHANNELS];

// The resulting centimeter values
int irCm[ADC_CHANNELS];

// ADC config.
// Note that changing autotuxhardware.h should be enough on pin layout change.
// Channels:
// 6 = side front
// 14 = side rear
// 15 = rear
static const ADCConversionGroup adc_group = {
  false, // No circular buffer (stop after filling the buffer each time)
  ADC_CHANNELS,
  adcCallback, // Callback
  NULL,
  0,
  ADC_CR2_SWSTART,
  ADC_SAMPLE_RATES,
  0,
  ADC_SQR1_NUM_CH(ADC_CHANNELS),
  0,
  ADC_SEQUENCE
};


//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------


/*
 * Sets up the IR sensor pins etc.
 */
void hardwareSetupIR() {
	adcStart(&ADCD1, NULL);
	for (int i = 0; i < ADC_CHANNELS; i++) {
		palSetPadMode(ADC_PIN_GROUPS[i], ADC_PIN_NUMBERS[i], PAL_MODE_INPUT_ANALOG);
	}
}

/*
 * Call this each time an analog read should be performed.
 */
void hardwareIterationIR() {
	adcStartConversion(&ADCD1, &adc_group, &samples[0], ADC_SAMPLES);
}


/*
 * Getter for the values. Specify an IR sensor.
 */


//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------


/*
 * Callback function, runs when ADC is completed.
 */
void adcCallback(ADCDriver *adcp, adcsample_t *buffer, size_t n) {
  (void) buffer; (void) n;

  if (adcp->state == ADC_COMPLETE) {
    	/* Calculates the average values from the ADC samples.*/
		avg[0] = (samples[0] + samples[3] + samples[6] + samples[9]) / 4;
		avg[1] = (samples[1] + samples[4] + samples[7] + samples[10]) / 4;
		avg[2] = (samples[2] + samples[5] + samples[8] + samples[11]) / 4;
		avg[0] = avg[0] >> 3;
		avg[1] = avg[1] >> 3;
		avg[2] = avg[2] >> 3;
		cm[0] = (int)(2914.0f / (avg[0] + 5.0f))- 1;
		cm[1] = (int)(2914.0f / (avg[1] + 5.0f))- 1;
		cm[2] = (int)(2914.0f / (avg[2] + 5.0f))- 1;
	}
}
