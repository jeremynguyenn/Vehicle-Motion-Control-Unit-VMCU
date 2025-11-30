/*
 * scs.h
 *
 *      Author: Nguyen Nhan
 */

#ifndef INC_TER_SCS_H_
#define INC_TER_SCS_H_

#include <string.h>  // For memset
#include "stm32f4xx.h"  // For bare-metal TIM registers
#include "TeR_CAN.h"  // For CAN interaction

// Define critical signals
#define SCS {TER_APPS_FRAME_ID} // Add critical signals here
#define SCS_TIMEOUT 600  // Defines the maximum time in timer units (ms) that an SCS can deviate

// Function prototypes
uint8_t initSCS(TIM_TypeDef *timBase); // Takes a timebase (Timer@1khz) and a checking interrupt
uint8_t logSCS(uint32_t id); // Checks if the ID is in the list and logs its timestamp

void checkSCS(void); // Checks if any ID is expired relative to the current timestamp value, returns its ID or 0 if okay
uint8_t startSCS(void); // Starts the SCS system
uint8_t stopSCS(void); // Stops the SCS system

#endif /* INC_TER_SCS_H_ */