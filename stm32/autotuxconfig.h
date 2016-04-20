/*
 * autotuxhardware.h
 * For all hardware config needs.
 */

#ifndef AUTOTUXCONFIG_H_
#define AUTOTUXCONFIG_H_

#include "hal.h"




//-----------------------------------------------------------------------------
// Serial behaviour
//-----------------------------------------------------------------------------


// Debug output. Set to 0 for normal packet output.
#define DEBUG_OUTPUT 0

// If more iterations than this occurs without receiving a valid packet,
// the car is stopped and wheels are centered.
#define MAX_ITERATIONS_WITHOUT_RECEIVE 5

// If more bytes than this is received in the same iteration, we know we are
// either getting lots of garbage data or we have a serious issue with keeping
// a good communication rate with the Odroid. The car will stop and the wheels
// will be centered.
#define MAX_RECEIVE_BYTES_IN_ITERATION 200


//-----------------------------------------------------------------------------
// RC mode behavior. If recalibrating PWM values, also see PWM hardware config.
//-----------------------------------------------------------------------------


// Use this to attenuate values from RC transmitter
// BE CAREFUL, DON'T USE VALUES HIGHER THAN ONE UNLESS YOU KNOW WHAT YOU'RE DOING
#define RC_FORWARD_MULTIPLIER 0.2
#define RC_BACKWARD_MULTIPLIER 0.7

// When RC is on, the throttle signal is above this treshold value
#define RC_THROTTLE_ON_TRESHOLD 100

// Anything below this will be BRAKE when in non-RC mode
#define RC_THROTTLE_BRAKE_TRESHOLD 1000

// Number of consecutive iterations to steer in the extreme directions to change mode
#define ITERATIONS_TO_CHANGE_MODE 20
// Number of iterations you have to center to lock in the mode change
#define ITERATIONS_TO_CENTER 10

// Steer max right to activate RC mode
#define RC_STEERING_ACTIVATION_TRESHOLD 2060
// Steer max left to deactivate RC mode
#define RC_STEERING_DEACTIVATION_TRESHOLD 1140


//-----------------------------------------------------------------------------
// IR hardware config
//-----------------------------------------------------------------------------

#define IR_VALUE_CAP 25

// Used to identify each sensor
typedef enum {IR_SIDE_FRONT, IR_SIDE_REAR, IR_REAR} IR_SENSOR;

// Pin config. Group/port + pin number. Now B0, C4, C5.
static const ioportid_t ADC_PIN_GROUPS[] = {GPIOB, GPIOC, GPIOC};
static const ioportmask_t ADC_PIN_NUMBERS[] = {0, 4, 5};

// Number of channels of ADC
#define ADC_CHANNELS 3

// Samples per channel
#define ADC_SAMPLES 10

// Sample channels and rates
#define ADC_SAMPLE_RATES ADC_SMPR2_SMP_AN8(ADC_SAMPLE_480) | ADC_SMPR1_SMP_AN14(ADC_SAMPLE_480) | ADC_SMPR1_SMP_AN15(ADC_SAMPLE_480)

// Sequence for reading the ADC channels
#define ADC_SEQUENCE ADC_SQR3_SQ1_N(ADC_CHANNEL_IN8) | ADC_SQR3_SQ2_N(ADC_CHANNEL_IN14) | ADC_SQR3_SQ3_N(ADC_CHANNEL_IN15)


//-----------------------------------------------------------------------------
// US hardware config
//-----------------------------------------------------------------------------

typedef enum {US_FRONT, US_SIDE} US_SENSOR;

#define US_SENSORS 2

static const uint8_t US_ADDRESS[] = {0x70, 0x73};

// Pins C9, A8
static const ioportid_t US_PIN_GROUPS[] = {GPIOC, GPIOA};
static const ioportmask_t US_PIN_NUMBERS[] = {9, 8};

#define US_I2C_DRIVER &I2CD3

#define US_VALUE_CAP 100

//-----------------------------------------------------------------------------
// RC hardware config
//-----------------------------------------------------------------------------


typedef enum {THROTTLE, STEERING} RC_CHANNEL;

#define RC_TIMER_THROTTLE &ICUD3
#define RC_TIMER_CHANNEL_THROTTLE ICU_CHANNEL_1
#define RC_TIMER_STEERING &ICUD4
#define RC_TIMER_CHANNEL_STEERING ICU_CHANNEL_1

#define RC_CHANNELS 2

// B4, B6
static const ioportid_t RC_PIN_GROUPS[] = {GPIOB, GPIOB};
static const ioportmask_t RC_PIN_NUMBERS[] = {4, 6};


//-----------------------------------------------------------------------------
// WE hardware config
//-----------------------------------------------------------------------------


#define WE_TICKS_PER_METER 44.07

// A2
static const ioportid_t WE_PIN_GROUP = GPIOA;
static const ioportmask_t WE_PIN_NUMBER  = 2;


//-----------------------------------------------------------------------------
// PWM hardware config and pulsewidth values
//-----------------------------------------------------------------------------

static const ioportid_t PWM_PIN_GROUPS[] = {GPIOA, GPIOA};
static const ioportmask_t PWM_PIN_NUMBERS[] = {0, 1};

#define PWM_TIMER &PWMD5

typedef enum {PWM_OUTPUT_ESC, PWM_OUTPUT_SERVO} PWM_OUTPUT_ID;

// Speeds and their corresponding pulsewidths.
typedef enum {SPEED_REVERSE, SPEED_STOP, SPEED_FORWARD_LOW, SPEED_FORWARD_CRUISE} SPEED;
#define SPEED_STEPS 4
static const int SPEED_PULSEWIDTHS[SPEED_STEPS] = {1140, 1350, 1410, 1440};

#define WHEELS_MAXLEFT_PW 1220
#define WHEELS_CENTERED_PW 1590
#define WHEELS_MAXRIGHT_PW 1980

#define WHEELS_MAXLEFT_ANGLE 60
#define WHEELS_CENTERED_ANGLE 90
#define WHEELS_MAXRIGHT_ANGLE 120




#endif /* AUTOTUXCONFIG_H_ */
