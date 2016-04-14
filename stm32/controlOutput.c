/**
 * Control output
 */


// ChibiOS includes
#include <ch.h>
#include <hal.h>

// Local includes
#include "autotuxconfig.h"
#include "controlOutput.h"
#include "hardwarePWM.h"
#include "hardwareRC.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// When RC is on, the throttle signal is above this treshold value
#define RC_THROTTLE_ON_TRESHOLD 100

// Anything below this will be BRAKE when in non-RC mode
#define RC_THROTTLE_BRAKE_TRESHOLD 1000

// Number of consecutive iterations to steer in the extreme directions to change mode
#define ITERATIONS_TO_CHANGE_MODE 10

// Steer max right to activate RC mode
#define RC_STEERING_ACTIVATION_TRESHOLD 2060
// Steer max left to deactivate RC mode
#define RC_STEERING_DEACTIVATION_TRESHOLD 1140

bool handleRCMode(void);
bool rcModeCheck(void);

//-----------------------------------------------------------------------------
// "Public" interface
//-----------------------------------------------------------------------------


/**
 * Initializes everything related to control output
 */
void controlOutputSetup(void) {
	// TODO: also initialize RC here later

	hardwareSetupPWM();
}


/**
 * Engine stopped, wheels centered (unless in RC mode).
 */
void controlOutputStopCenter(void) {
	if (!handleRCMode()) {
		hardwareSetValuesPWM(PWM_OUTPUT_ESC, SPEED_STOP);
		hardwareSetValuesPWM(PWM_OUTPUT_SERVO, WHEELS_CENTERED_ANGLE);
	}
}


/**
 * Output control data to engine and wheels (unless in RC mode).
 */
void controlOutput(unsigned char* controlData) {
	if (!handleRCMode()) {
		// RC mode not activated - the only RC input we care about is braking
		if (hardwareGetValuesRC(THROTTLE) >= RC_THROTTLE_ON_TRESHOLD &&
				hardwareGetValuesRC(THROTTLE) <= RC_THROTTLE_BRAKE_TRESHOLD) {

			// Stop the car
			hardwareSetValuesPWM(PWM_OUTPUT_ESC, SPEED_STOP);

		} else {
			// Drive according to controlData
			// Speed controlled by int corresponding to SPEED enum in config
			hardwareSetValuesPWM(PWM_OUTPUT_ESC, controlData[CONTROL_BYTE_SPEED]);

			// Wheel angle: 90 degress +- ~30 degrees.
			hardwareSetValuesPWM(PWM_OUTPUT_SERVO, controlData[CONTROL_BYTE_ANGLE]);
		}
	}
}


//-----------------------------------------------------------------------------
// "Private" implementation
//-----------------------------------------------------------------------------


/**
 * Returns true if RC mode is active and RC signal was forwarded to hardware.
 * Returns false if RC mode off.
 */
bool handleRCMode(void) {
	if (hardwareGetValuesRC(THROTTLE) > RC_THROTTLE_ON_TRESHOLD) {
		if (rcModeCheck()) {
			// Forward RC signal to hardware
			hardwareSetValuesPWM_RC(hardwareGetValuesRC(THROTTLE), hardwareGetValuesRC(STEERING));

			// Blue LED for RC mode
			palSetPad(GPIOD, GPIOD_LED6);
			return true;

		}
	}
	// Blue LED off
	palClearPad(GPIOD, GPIOD_LED6);
	return false;
}

/**
 * Turns RC mode on or off based on the amount of consecutive iterations
 * with the values from the RC transmitter above or under certain treshold values.
 */
bool rcModeCheck(void) {
	static int itAboveActivationTreshold = 0;
	static int itBelowDeactivationTreshold = 0;
	static bool rcMode = false;
	icucnt_t steeringPW = hardwareGetValuesRC(STEERING);

	// Only check for activation of RC mode if not already active
	if (!rcMode && steeringPW > RC_STEERING_ACTIVATION_TRESHOLD) {
		// Increase counter if we're on the way towards activation
		if (itAboveActivationTreshold < ITERATIONS_TO_CHANGE_MODE) {
			itAboveActivationTreshold++;
		} else {
			// Already enough iterations. Mode on
			rcMode = true;
		}
	} else if (rcMode && steeringPW < RC_STEERING_DEACTIVATION_TRESHOLD) {
		// Increase counter if we're on the way towards deactivation
		if (rcMode && itBelowDeactivationTreshold < ITERATIONS_TO_CHANGE_MODE) {
			itBelowDeactivationTreshold++;
		} else {
			// Already enough iterations. Mode off
			rcMode = false;
		}
	}

	// Reset counters if not in range
	if (steeringPW >= RC_STEERING_DEACTIVATION_TRESHOLD) {
		itBelowDeactivationTreshold = 0;
	} else if (steeringPW <= RC_STEERING_ACTIVATION_TRESHOLD) {
		itAboveActivationTreshold = 0;
	}
	return rcMode;
}
