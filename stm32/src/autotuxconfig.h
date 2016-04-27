/*
 * autotuxhardware.h
 * For all hardware config needs.
 */

#ifndef AUTOTUXCONFIG_H_
#define AUTOTUXCONFIG_H_

#include "hal.h"




//-----------------------------------------------------------------------------
// Serial and control out behaviour
//-----------------------------------------------------------------------------


// Debug output. Set to 0 for normal packet output.
#define DEBUG_OUTPUT 0

#if DEBUG_OUTPUT == 1
#define DEBUG
#endif

// If more iterations than this occurs without receiving a valid packet,
// the serial stops sending to avoid filling buffers. The car is also
// stopped and wheels are centered.

#ifdef DEBUG
#define MAX_ITERATIONS_WITHOUT_RECEIVE 200
#else
#define MAX_ITERATIONS_WITHOUT_RECEIVE 5
#endif

// If more bytes than this is received in the same iteration, we know we are
// either getting lots of garbage data or we have a serious issue with keeping
// a good communication rate with the Odroid. The car will stop and the wheels
// will be centered.
#define MAX_RECEIVE_BYTES_IN_ITERATION 200


/**
 * Actual data bytes in the control packet
 */
#define CONTROL_DATA_BYTES 2

/**
 * Size of the whole control data packet including header and end delimiter
 */
#define CONTROL_DATA_PACKET_SIZE 6

/**
 * Actual data bytes in the sensor data packet
 */
#define SENSOR_DATA_BYTES 11


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
#define ADC_SAMPLES 4

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

#define US_VALUE_CAP 90

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


#define WE_TICKS_PER_METER 68.49 // 44.07

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
static const int SPEED_PULSEWIDTHS[SPEED_STEPS] = {1130, 1350, 1425, 1440};

#define WHEELS_MAXLEFT_PW 1220
#define WHEELS_CENTERED_PW 1590
#define WHEELS_MAXRIGHT_PW 1980

#define WHEELS_MAXLEFT_ANGLE 60
#define WHEELS_CENTERED_ANGLE 90
#define WHEELS_MAXRIGHT_ANGLE 120


//-----------------------------------------------------------------------------
// LED config
//-----------------------------------------------------------------------------

#define LED_PORT GPIOA
#define LED_PIN 6

#define HEADLIGHT_LEDS LED(1) | LED(2) | LED(5) | LED(6)
#define HEADLIGHT_R 30
#define HEADLIGHT_G 30
#define HEADLIGHT_B 30

#define TAILLIGHT_LEDS LED(9) | LED(14)
#define TAILLIGHT_R 40
#define TAILLIGHT_G 0
#define TAILLIGHT_B 0

#define ITERATIONS_PER_FLASH_STATE 4
#define FLASHLEFT_LEDS LED(7) | LED(8)
#define FLASHRIGHT_LEDS LED(0) | LED(15)
#define FLASH_R 230
#define FLASH_G 130
#define FLASH_B 0

#define REVERSE_LEDS LED(11)
#define REVERSE_R 30
#define REVERSE_G 30
#define REVERSE_B 30

#define BRAKE_LEDS LED(10) | LED(13)
#define BRAKE_R 255
#define BRAKE_G 0
#define BRAKE_B 0

#define RCLIGHT_LEDS LED(3) | LED(4) | LED(12)
#define RCLIGHT_R 0
#define RCLIGHT_G 0
#define RCLIGHT_B 150


#endif /* AUTOTUXCONFIG_H_ */
