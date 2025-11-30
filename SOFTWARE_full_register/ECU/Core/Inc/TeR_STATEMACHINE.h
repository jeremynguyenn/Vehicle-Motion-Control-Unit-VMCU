/*
 * stateMachine.h
 *
 *  Created on: Feb 1, 2024
 *      Author: Ozuba
 *
 * The approach of consulting all conditions before executing the state has been chosen
 */

#ifndef INC_TER_STATEMACHINE_H_
#define INC_TER_STATEMACHINE_H_

#include "stm32f4xx.h"  // For bare-metal register access
#include "main.h"  // For pin definitions
#include "TeR_CAN.h"  // For CAN structures
#include "TeR_TRQMANAGER.h"  // For torque management
#include "TeR_REFRI.h"  // For refrigeration
#include "TeR_CONSTANTS.h"  // For constants like PI
#include "TeR_UTILS.h"  // For persistence checking

// State enumeration
typedef enum {
	WAIT_SL, RDY2PRECH, PRECHARGING, PRECHARGED, DRIVING
} state_t; // States

// Function prototypes
uint8_t initStateMachine(TIM_TypeDef *htim); // Starts the machine with a timer
state_t getState(void); // Determines the state based on readings and CAN
void stateMachine(TIM_TypeDef *beat); // Executes the current state

// State functions
void waitSL(void); // Checks if safety loop is okay
void rdy2Prech(void); // Waits to receive the precharge command
void precharging(void); // Transitional state, monitors that everything is going well
void precharged(void); // Waits to receive the ready-to-drive command
void driving(void); // Executes the torque command

// Permanent task
void permaTask(void); // Executes in all states (conversions/brakelight...)

#endif /* INC_TER_STATEMACHINE_H_ */