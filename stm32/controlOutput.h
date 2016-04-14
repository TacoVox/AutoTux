/*
 * sensorInput.h
 */

#ifndef CONTROLOUTPUT_H_
#define CONTROLOUTPUT_H_


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
void controlOutput(unsigned char* controlData);


#endif /* CONTROLOUTPUT_H_ */
