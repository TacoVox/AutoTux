/** @file	autotuxconfig.h
 * 	@brief	Main config file for AutoTux.
 */


#ifndef AUTOTUXCONFIG_H_
#define AUTOTUXCONFIG_H_


#include "hal.h"


//-----------------------------------------------------------------------------
// Serial and control out behaviour
//-----------------------------------------------------------------------------


/**
 * Debug output. Set to 0 for normal packet output.
 */
#define DEBUG_OUTPUT 0

/**
 * @brief If this is exceeded, communication is seen as inactive.
 *
 * If more iterations than this occurs without receiving a valid packet,
 * the serial stops sending to avoid filling buffers.
 */
#ifdef DEBUG
#define MAX_ITERATIONS_WITHOUT_RECEIVE 200
#else
#define MAX_ITERATIONS_WITHOUT_RECEIVE 5
#endif

/**
 * @brief Max bytes to read in an iteration.
 *
 * If more bytes than this is received in the same iteration, we know we are
 * either getting lots of garbage data or we have a serious issue with keeping
 * a good communication rate with the Odroid. The limit protects from getting
 * stuck in the reading loop.
 */
#define MAX_RECEIVE_BYTES_IN_ITERATION 200

/**
 * Actual data bytes in the control packet
 */
#define CONTROL_DATA_BYTES 3

/**
 * Size of the whole control data packet including header and end delimiter
 */
#define CONTROL_DATA_PACKET_SIZE 7

/**
 * Actual data bytes in the sensor data packet
 */
#define SENSOR_DATA_BYTES 11


//-----------------------------------------------------------------------------
// RC mode behavior. If recalibrating PWM values, also see PWM hardware config.
//-----------------------------------------------------------------------------


/**
 * @brief Use this to attenuate values from RC transmitter. BE CAREFUL.
 *
 * DON'T USE VALUES HIGHER THAN ONE UNLESS YOU KNOW WHAT YOU'RE DOING.
 */
#define RC_FORWARD_MULTIPLIER 0.2
#define RC_BACKWARD_MULTIPLIER 0.7

/**
 * When RC is on, the throttle signal is above this treshold value
 */
#define RC_THROTTLE_ON_TRESHOLD 100

/**
 * Anything below this will be BRAKE when in non-RC mode
 */
#define RC_THROTTLE_BRAKE_TRESHOLD 1000

/**
 * Number of consecutive iterations to steer in the extreme directions to change mode
 */
#define RC_ITERATIONS_TO_CHANGE_MODE 20

/**
 * Number of iterations you have to center to lock in the mode change
 */
#define RC_ITERATIONS_TO_CENTER 10

/**
 * Steer max right to activate RC mode
 */
#define RC_STEERING_ACTIVATION_TRESHOLD 2060

/**
 * Steer max left to deactivate RC mode
 */
#define RC_STEERING_DEACTIVATION_TRESHOLD 1140


//-----------------------------------------------------------------------------
// IR hardware config
//-----------------------------------------------------------------------------


/**
 * Cap for IR values, to clean up the signal.
 */
#define IR_VALUE_CAP 25

/**
 * Used to identify each IR sensor
 */
typedef enum {IR_SIDE_FRONT, IR_SIDE_REAR, IR_REAR} IR_SENSOR;

/**
 * IR Pin config. Group/port + pin number. Now B0, C4, C5.
 */
static const ioportid_t IR_ADC_PIN_GROUPS[] = {GPIOB, GPIOC, GPIOC};
static const ioportmask_t IR_ADC_PIN_NUMBERS[] = {0, 4, 5};

/**
 * Number of channels of ADC (IR sensors)
 */
#define IR_ADC_CHANNELS 3

/**
 * Samples per channel (IR sensors)
 */
#define IR_ADC_SAMPLES 4

/**
 * Sample channels and rates (IR sensors)
 */
#define IR_ADC_SAMPLE_RATES ADC_SMPR2_SMP_AN8(ADC_SAMPLE_480) | ADC_SMPR1_SMP_AN14(ADC_SAMPLE_480) | ADC_SMPR1_SMP_AN15(ADC_SAMPLE_480)

/**
 * Sequence for reading the ADC channels (IR sensors)
 */
#define IR_ADC_SEQUENCE ADC_SQR3_SQ1_N(ADC_CHANNEL_IN8) | ADC_SQR3_SQ2_N(ADC_CHANNEL_IN14) | ADC_SQR3_SQ3_N(ADC_CHANNEL_IN15)


//-----------------------------------------------------------------------------
// US hardware config
//-----------------------------------------------------------------------------


/**
 * Value cap for US sensors. Used to reduce noise.
 */
#define US_VALUE_CAP 90

/**
 * Used to identify each US sensor
 */
typedef enum {US_FRONT, US_SIDE} US_SENSOR;

/**
 * Amount of US sensors
 */
#define US_SENSORS 2

/**
 * I2C Addresses of the US sensors
 */
static const uint8_t US_ADDRESS[] = {0x70, 0x73};

/**
 * Pins for I2C. Currently pins C9, A8
 */
static const ioportid_t US_PIN_GROUPS[] = {GPIOC, GPIOA};
static const ioportmask_t US_PIN_NUMBERS[] = {9, 8};

/**
 * I2C driver. Currently channel 3.
 */
#define US_I2C_DRIVER &I2CD3


//-----------------------------------------------------------------------------
// RC hardware config
//-----------------------------------------------------------------------------


/**
 * Used to identify RC channels
 */
typedef enum {THROTTLE, STEERING} RC_CHANNEL;

/**
 * Timer / ICU driver for throttle
 */
#define RC_TIMER_THROTTLE &ICUD3

/**
 * Timer / ICU channel for throttle
 */
#define RC_TIMER_CHANNEL_THROTTLE ICU_CHANNEL_1

/**
 * Timer / ICU driver for throttle
 */
#define RC_TIMER_STEERING &ICUD4

/**
 * Timer / ICU channel for throttle
 */
#define RC_TIMER_CHANNEL_STEERING ICU_CHANNEL_1

/**
 * Amount of RC channels
 */
#define RC_CHANNELS 2

/**
 * Pins for RC in. B4, B6
 */
static const ioportid_t RC_PIN_GROUPS[] = {GPIOB, GPIOB};
static const ioportmask_t RC_PIN_NUMBERS[] = {4, 6};


//-----------------------------------------------------------------------------
// WE hardware config
//-----------------------------------------------------------------------------


/**
 * Ticks per meter. Used for distance and speed calculation.
 */
#define WE_TICKS_PER_METER 50 // 68.49 // 44.07

/**
 * Pin for wheel encoder. Currently A2.
 */
static const ioportid_t WE_PIN_GROUP = GPIOA;
static const ioportmask_t WE_PIN_NUMBER  = 2;


//-----------------------------------------------------------------------------
// PWM hardware config and pulsewidth values
//-----------------------------------------------------------------------------


/**
 * Pins for PWM out. Currently A0, A1.
 */
static const ioportid_t PWM_PIN_GROUPS[] = {GPIOA, GPIOA};
static const ioportmask_t PWM_PIN_NUMBERS[] = {0, 1};

/**
 * PWM timer.
 */
#define PWM_TIMER &PWMD5

/**
 * Used to identify PWM outputs.
 */
typedef enum {PWM_OUTPUT_ESC, PWM_OUTPUT_SERVO} PWM_OUTPUT_ID;

/**
 * Speed step names.
 */
typedef enum {SPEED_REVERSE, SPEED_STOP, SPEED_FORWARD_LOW, SPEED_FORWARD_CRUISE} CTRL_OUT_SPEED;

/**
 * Amount of speed steps.
 */
#define CTRL_OUT_SPEED_STEPS 4

/**
 * Pulse widths for each speed step. 1414 very low forward speed good for basic lf.
 */
static const int CTRL_OUT_SPEED_PULSEWIDTHS[CTRL_OUT_SPEED_STEPS] = {1120, 1350, 1415, 1416};

/**
 * Pulse width values for wheels centered and their extreme positions.
 */
#define CTRL_OUT_WHEELS_CENTERED_PW 1590
#define CTRL_OUT_WHEELS_MAXLEFT_PW 1220
#define CTRL_OUT_WHEELS_MAXRIGHT_PW 1980

/**
 * @brief Angles for wheels centered and their extreme positions.
 *
 * (We treat 90 degrees as wheels centered.)
 */
#define CTRL_OUT_WHEELS_MAXLEFT_ANGLE 60
#define CTRL_OUT_WHEELS_CENTERED_ANGLE 90
#define CTRL_OUT_WHEELS_MAXRIGHT_ANGLE 120


//-----------------------------------------------------------------------------
// Lights config
//-----------------------------------------------------------------------------


/**
 * LED pin. Currently A6.
 */
#define LIGHT_LED_PORT GPIOA
#define LIGHT_LED_PIN 6

/**
 * Head light configuration.
 */
#define LIGHT_HEADLIGHT_LEDS LED(1) | LED(2) | LED(5) | LED(6)
#define LIGHT_HEADLIGHT_R 30
#define LIGHT_HEADLIGHT_G 30
#define LIGHT_HEADLIGHT_B 30

#define LIGHT_HEADLIGHT_HI_R 150
#define LIGHT_HEADLIGHT_HI_G 150
#define LIGHT_HEADLIGHT_HI_B 150
#define LIGHT_HEADLIGHT_TRESHOLD 60

/**
 * Tail light configuration.
 */
#define LIGHT_TAILLIGHT_LEDS LED(9) | LED(14)
#define LIGHT_TAILLIGHT_R 30
#define LIGHT_TAILLIGHT_G 0
#define LIGHT_TAILLIGHT_B 0

/**
 * Flash light configuration.
 */
#define LIGHT_ITERATIONS_PER_FLASH_STATE 4
#define LIGHT_FLASHLEFT_LEDS LED(7) | LED(8)
#define LIGHT_FLASHRIGHT_LEDS LED(0) | LED(15)
#define LIGHT_FLASH_R 230
#define LIGHT_FLASH_G 90
#define LIGHT_FLASH_B 0

/**
 * Reverwse light configuration.
 */
#define LIGHT_REVERSE_LEDS LED(11)
#define LIGHT_REVERSE_R 30
#define LIGHT_REVERSE_G 30
#define LIGHT_REVERSE_B 30

/**
 * Brake light configuration.
 */
#define LIGHT_BRAKE_LEDS LED(10) | LED(13)
#define LIGHT_BRAKE_R 255
#define LIGHT_BRAKE_G 0
#define LIGHT_BRAKE_B 0

/**
 * RC light configuration.
 */
#define LIGHT_RCLIGHT_LEDS LED(3) | LED(4) | LED(12)
#define LIGHT_RCLIGHT_R 0
#define LIGHT_RCLIGHT_G 0
#define LIGHT_RCLIGHT_B 80


#endif /* AUTOTUXCONFIG_H_ */
