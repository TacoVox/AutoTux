/*
 * autotuxhardware.h
 * For all hardware config needs.
 */

#ifndef AUTOTUXHARDWARE_H_
#define AUTOTUXHARDWARE_H_

#include "hal.h"


//-----------------------------------------------------------------------------
// IR config
//-----------------------------------------------------------------------------

// Used to identify each sensor
typedef enum {SIDE_FRONT, SIDE_REAR, REAR} IR_SENSOR;

// Pin config. Group/port + pin number. Now B0, C4, C5.
static const ioportid_t ADC_PIN_GROUPS[] = {GPIOB, GPIOC, GPIOC};
static const ioportmask_t ADC_PIN_NUMBERS[] = {0, 4, 5};

// Number of channels of ADC
#define ADC_CHANNELS 3

// Samples per channel
#define ADC_SAMPLES 4

// Sample channels and rates
#define ADC_SAMPLE_RATES ADC_SMPR2_SMP_AN8(ADC_SAMPLE_480) | ADC_SMPR1_SMP_AN14(ADC_SAMPLE_480) | ADC_SMPR1_SMP_AN15(ADC_SAMPLE_480)

// Sequence for reading the ADC channels
#define ADC_SEQUENCE ADC_SQR3_SQ1_N(ADC_CHANNEL_IN8) | ADC_SQR3_SQ2_N(ADC_CHANNEL_IN14) | ADC_SQR3_SQ3_N(ADC_CHANNEL_IN15)


//-----------------------------------------------------------------------------
// US config - NOTE ideally move config of pins and channels here as well
//-----------------------------------------------------------------------------

typedef enum {FRONT, SIDE} US_SENSOR;

#define US_SENSORS 2

static const uint8_t US_ADDRESS[] = {0x79, 0x73};


//-----------------------------------------------------------------------------
// RC config
//-----------------------------------------------------------------------------

typedef enum {THROTTLE, STEERING} RC_CHANNEL;

#define RC_CHANNELS 2


#endif /* AUTOTUXHARDWARE_H_ */
