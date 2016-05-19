/** @file	hardwareIR.c
 * 	@brief Reads the IR sensors.
 *
 * Uses ADC with several samples for each measurements.
 */


#include "hal.h"
#include "hardwareIR.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

static void adcCallback(ADCDriver *adcp, adcsample_t *buffer, size_t n);

/**
 *	Sample buffer array
 */
static adcsample_t irSamples[IR_ADC_SAMPLES * IR_ADC_CHANNELS] = {0};

/**
 * Array for averages
 */
static adcsample_t irAvg[IR_ADC_CHANNELS];

/**
 * The resulting centimeter values
 */
static int irCm[IR_ADC_CHANNELS];

/**
 * ADC group config
 */
static const ADCConversionGroup adc_group = {
  false, // No circular buffer (stop after filling the buffer each time)
  IR_ADC_CHANNELS,
  adcCallback, // Callback
  NULL,
  0,
  ADC_CR2_SWSTART,
  IR_ADC_SAMPLE_RATES,
  0,
  ADC_SQR1_NUM_CH(IR_ADC_CHANNELS),
  0,
  IR_ADC_SEQUENCE
};


//-----------------------------------------------------------------------------
// Public interface
//-----------------------------------------------------------------------------


/*
 * Sets up the IR sensor pins etc.
 */
void hardwareIRSetup() {
	adcStart(&ADCD1, NULL);
	for (int i = 0; i < IR_ADC_CHANNELS; i++) {
		palSetPadMode(IR_ADC_PIN_GROUPS[i], IR_ADC_PIN_NUMBERS[i], PAL_MODE_INPUT_ANALOG);
	}
}

/*
 * Call this each time an analog read should be performed.
 */
void hardwareIRIteration() {
	adcStartConversion(&ADCD1, &adc_group, &irSamples[0], IR_ADC_SAMPLES);
}

/*
 * Getter for the values. Specify an IR sensor.
 */
int hardwareIRGetValues(IR_SENSOR sensor) {
	return (irCm[sensor] < IR_VALUE_CAP) ? irCm[sensor] : IR_VALUE_CAP;
}


//-----------------------------------------------------------------------------
// Implementation. The static functions below are inaccessible to other modules
//-----------------------------------------------------------------------------


/*
 * @brief Callback function, runs when ADC is completed.
 *
 * (Could be generalized with a loop)
 */
void adcCallback(ADCDriver *adcp, adcsample_t *buffer, size_t n) {
  (void) buffer; (void) n;

  if (adcp->state == ADC_COMPLETE) {
    	/* Calculates the average values from the ADC irSamples.*/
		irAvg[0] = (irSamples[0] + irSamples[3] + irSamples[6] + irSamples[9]) / 4;
		irAvg[1] = (irSamples[1] + irSamples[4] + irSamples[7] + irSamples[10]) / 4;
		irAvg[2] = (irSamples[2] + irSamples[5] + irSamples[8] + irSamples[11]) / 4;
		// Conversion algorithm from Arduino. Bit shift to compensate for the higher
		// ADC resolution on STM
		irAvg[0] = irAvg[0] >> 3;
		irAvg[1] = irAvg[1] >> 3;
		irAvg[2] = irAvg[2] >> 3;
		// Formula: irCm[0] = (int)(2914.0f / (irAvg[0] + 5.0f))- 1;
		// Note: roughly calibrated with -3 as a constant instead of -1
		// Here we also add the previous value and divide by 2 to average out
		irCm[0] = (irCm[0] + (int)(2914.0f / (irAvg[0] + 5.0f))- 3) / 2;
		irCm[1] = (irCm[1] + (int)(2914.0f / (irAvg[1] + 5.0f))- 3) / 2;
		irCm[2] = (irCm[2] + (int)(2914.0f / (irAvg[2] + 5.0f))- 3) / 2;

  }
}
