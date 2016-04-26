/*
 * sensorInput.h
 */

#ifndef CONTROLOUTPUT_H_
#define CONTROLOUTPUT_H_


#define DEFAULT_CONTROL_BYTES {SPEED_STOP, WHEELS_CENTERED_ANGLE}
#define CONTROL_BYTE_COUNT 2
typedef enum {CONTROL_BYTE_SPEED, CONTROL_BYTE_ANGLE} CONTROL_BYTES;


/**
 * Initializes everything related to control output
 */
void controlOutputSetup(void);


/**
 * Engine stopped, wheels centered.
 */
void controlOutputStopCenter(void);


/**
 * Output control data to engine and wheels
 */
void controlOutputIteration(void);


/**
 * Set control data based on received packet
 */
void controlOutputSetData(unsigned char* controlData);


#endif /* CONTROLOUTPUT_H_ */
