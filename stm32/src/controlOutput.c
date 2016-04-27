/** @file	controlOutput.c
 * 	@brief	Handles the output of control data to the hardware.
 *
 *  Control data is received by the serialConnection which calls the setter
 *  controlOutputSetData in this module. In case any problems occur with the
 *  communication, this module will make sure that if no valid values are
 *  received for a while, the car will 	stop.
 */


// ChibiOS includes
#include <ch.h>
#include <hal.h>

// Local includes
#include "autotuxconfig.h"
#include "controlOutput.h"
#include "hardware/hardwarePWM.h"
#include "hardware/hardwareRC.h"
#include "hardware/hardwareLights.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


static void controlOutputStopCenter(void);
static bool handleRCMode(void);
static bool rcModeCheck(void);


/**
 * Holds the latest control data received from the serial connection.
 */
static unsigned char controlData[CONTROL_DATA_BYTES];

/**
 * @brief Whether control values are so new they have not been output to HW yet.
 *
 * Used to notice whenever we haven't received valid communication for a while,
 * so we can take appropriate measure (stop the car, center wheels).
 */
static bool controlValuesAreNew = false;


//-----------------------------------------------------------------------------
// Public interface
//-----------------------------------------------------------------------------


/**
 * Initializes everything related to control output
 */
void controlOutputSetup(void) {
	// TODO: also initialize RC here later?
	hardwareSetupPWM();
	hardwareSetupLights();

	//hardwareIterationLights(0, false, false);
}

/**
 * Output control data to engine and wheels (unless in RC mode).
 */
void controlOutputSetData(unsigned char* newControlData) {
	// Value copy!
	for (int i = 0; i < CONTROL_DATA_BYTES; i++) {
		controlData[i] = newControlData[i];
	}

	// Touch this variable
	controlValuesAreNew = true;
}

/**
 * Output control data to engine and wheels (unless in RC mode).
 */
void controlOutputIteration() {
	static int iterationsNoNewValues = 0;
	bool rcBrake = false;
	bool rcMode = handleRCMode();

	if (!rcMode) {
		// RC mode not activated - check only for RC transmitter brake
		if (hardwareGetValuesRC(THROTTLE) >= RC_THROTTLE_ON_TRESHOLD &&
				hardwareGetValuesRC(THROTTLE) <= RC_THROTTLE_BRAKE_TRESHOLD) {

			// RC transmitter brake - stop the car
			hardwareSetValuesPWM(PWM_OUTPUT_ESC, SPEED_STOP);
			rcBrake = true;

		} else {
			// Normal automatic drive. See if we have newly received values
			if (controlValuesAreNew) {
				// Drive according to controlData
				// Speed controlled by int corresponding to SPEED enum in config
				hardwareSetValuesPWM(PWM_OUTPUT_ESC, controlData[CONTROL_BYTE_SPEED]);

				// Wheel angle: 90 degress +- ~30 degrees.
				hardwareSetValuesPWM(PWM_OUTPUT_SERVO, controlData[CONTROL_BYTE_ANGLE]);

				controlValuesAreNew = false;
				iterationsNoNewValues = 0;
			} else {
				// No new values! Worrying.
				if (iterationsNoNewValues > MAX_ITERATIONS_WITHOUT_RECEIVE) {
					// Stop the car and center wheels!
					controlOutputStopCenter();
				} else {
					iterationsNoNewValues++;
				}
			}
		}
	}

	// Update lights
	hardwareIterationLights(LIGHT_BIT_FLASH_LEFT, rcMode, rcBrake);
}


//-----------------------------------------------------------------------------
// Implementation. The static functions below are inaccessible to other modules
//-----------------------------------------------------------------------------


/**
 * Stop engine and center wheels (unless in RC mode).
 */
static void controlOutputStopCenter(void) {
	if (!handleRCMode()) {
		hardwareSetValuesPWM(PWM_OUTPUT_ESC, SPEED_STOP);
		hardwareSetValuesPWM(PWM_OUTPUT_SERVO, WHEELS_CENTERED_ANGLE);
	}

	// Regardless, reset controlData to corresponding values.
	controlData[CONTROL_BYTE_SPEED] = SPEED_STOP;
	controlData[CONTROL_BYTE_ANGLE] = WHEELS_CENTERED_ANGLE;
}

/**
 * Returns true if RC mode is active and RC signal was forwarded to hardware.
 * Returns false if RC mode off.
 */
static bool handleRCMode(void) {
	if (hardwareGetValuesRC(THROTTLE) > RC_THROTTLE_ON_TRESHOLD) {
		if (rcModeCheck()) {
			// Forward RC signal to hardware
			// But first attenuate speed
			int esc_pw = hardwareGetValuesRC(THROTTLE);
			if (esc_pw > SPEED_PULSEWIDTHS[SPEED_STOP]) {
				// Output a fifth of the input
				esc_pw = SPEED_PULSEWIDTHS[SPEED_STOP] +
						((esc_pw - SPEED_PULSEWIDTHS[SPEED_STOP]) * RC_FORWARD_MULTIPLIER);
			} else if (esc_pw < SPEED_PULSEWIDTHS[SPEED_STOP]) {
				// Attenuate backwards values by multiplying with 0.8
				esc_pw = SPEED_PULSEWIDTHS[SPEED_STOP] -
						((SPEED_PULSEWIDTHS[SPEED_STOP] - esc_pw) * RC_BACKWARD_MULTIPLIER);
			}

			hardwareSetValuesPWM_RC(esc_pw, hardwareGetValuesRC(STEERING));
			return true;
		}
	}
	return false;
}

/**
 * Turns RC mode on or off based on the amount of consecutive iterations
 * with the values from the RC transmitter above or under certain treshold values.
 */
static bool rcModeCheck(void) {
	static int itAboveActivationTreshold = 0;
	static int itBelowDeactivationTreshold = 0;
	static bool rcMode = false;
	static int itWindowToCenterToSwitch = 0;
	static bool ledState = false; // Led state for blinking
	icucnt_t steeringPW = hardwareGetValuesRC(STEERING);

	// Restore LED state - will be changed only if blinking occurs below
	if (rcMode) palSetPad(GPIOD, GPIOD_LED6);
	else palClearPad(GPIOD, GPIOD_LED6);

	// If not in window to activate/deactivate, do normal check for step 1
	if (itWindowToCenterToSwitch == 0) {
		// Step 1 check
		// Only check for activation of RC mode if not already active
		if (!rcMode && steeringPW > RC_STEERING_ACTIVATION_TRESHOLD) {
			// Increase counter if we're on the way towards activation
			if (itAboveActivationTreshold < ITERATIONS_TO_CHANGE_MODE) {
				itAboveActivationTreshold++;
			} else {
				// Already enough iterations. Step 1 complete
				//rcMode = true;
				itWindowToCenterToSwitch = ITERATIONS_TO_CENTER;
			}
		} else if (rcMode && steeringPW < RC_STEERING_DEACTIVATION_TRESHOLD) {
			// Increase counter if we're on the way towards deactivation
			if (rcMode && itBelowDeactivationTreshold < ITERATIONS_TO_CHANGE_MODE) {
				itBelowDeactivationTreshold++;
			} else {
				// Already enough iterations. Step 1 complete
				itWindowToCenterToSwitch  = ITERATIONS_TO_CENTER;
			}
		}

	} else {
		// Step 1 passed, waiting for step 2.
		// Blink LED and waiting for centering
		itAboveActivationTreshold = 0;
		itBelowDeactivationTreshold = 0;

		if (!ledState) {
			palSetPad(GPIOD, GPIOD_LED6);
			ledState = true;
		} else {
			palClearPad(GPIOD, GPIOD_LED6);
			ledState = false;
		}

		// Step 2 - executes if steering centered after step 1 complete
		if (itWindowToCenterToSwitch > 0 && steeringPW >
				WHEELS_CENTERED_PW - 50 && steeringPW < WHEELS_CENTERED_PW + 50) {
			// Toggle RC mode and close iteration window
			rcMode = !rcMode;
			itWindowToCenterToSwitch = 0;
		}

		// If step 1 complete but not step 2, keep diminishing window in which we wait
		// for centered steering
		if (itWindowToCenterToSwitch > 0) {
			itWindowToCenterToSwitch--;
		}
	}
	
	// Reset counters if not in range
	if (steeringPW >= RC_STEERING_DEACTIVATION_TRESHOLD) {
		itBelowDeactivationTreshold = 0;	
	}
	if (steeringPW <= RC_STEERING_ACTIVATION_TRESHOLD) {
		itAboveActivationTreshold = 0;
	}
	return rcMode;
}
