/*
 * hardwareLights.h
 *
 */

#ifndef HARDWARELIGHTS_H_
#define HARDWARELIGHTS_H_

#include "../autotuxconfig.h"


#define LIGHT_BIT_BRAKE (1 << 0)
#define LIGHT_BIT_REVERSE (1 << 1)
#define LIGHT_BIT_FLASH_LEFT (1 << 2)
#define LIGHT_BIT_FLASH_RIGHT (1 << 3)

/*
 * Sets up the light pin etc
 */
void hardwareSetupLights(void);

void hardwareIterationLights(unsigned char lightByte, bool rcMode, bool rcBrakeLight);



#endif /* HARDWARELIGHTS_H_ */
