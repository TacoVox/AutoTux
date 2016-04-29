/** @file	controlOutput.h
 * 	@brief	Handles the output of control data to the hardware.
 *
 *  Control data is received by the serialConnection which calls the setter
 *  controlOutputSetData in this module. In case any problems occur with the
 *  communication, this module will make sure that if no valid values are
 *  received for a while, the car will 	stop.
 */


#ifndef CONTROLOUTPUT_H_
#define CONTROLOUTPUT_H_


/**
 * The default control bytes (engine stopped, wheels centered).
 */
#define DEFAULT_CONTROL_BYTES {SPEED_STOP, WHEELS_CENTERED_ANGLE}

/**
 * Use these to easily access the correct byte in the control byte array.
 */
typedef enum {CONTROL_BYTE_SPEED, CONTROL_BYTE_ANGLE} CONTROL_BYTES;


/**
 * Initializes everything related to control output
 */
void controlOutputSetup(void);

/**
 * Output control data to engine and wheels
 */
void controlOutputIteration(void);

/**
 * Set control data based on received packet. Used by the serial connection module.
 */
void controlOutputSetData(unsigned char* controlData);


#endif /* CONTROLOUTPUT_H_ */
