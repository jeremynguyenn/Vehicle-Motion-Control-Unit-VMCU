/*
 * TeR_COMMAND.h
 *
 *      Author: Nguyen Nhan
 */
// Module that manages vehicle commands, contains the command function

#ifndef INC_TER_COMMAND_H_
#define INC_TER_COMMAND_H_

#include "TeR_CAN.h" // Necessary for interaction, exposes the CAN in use
#include "TeR_TRQMANAGER.h" // For configuring the torqueManager
#include "TeR_STATEMACHINE.h" // For the states
#include "TeR_SCS.h" // For activation/deactivation
#include "stm32f4xx.h"  // For bare-metal register access

// Function prototypes
uint8_t command(struct ter_command_t command); // Command function
uint8_t easyCommand(uint8_t cmd); // Wrapper for quick use of commands without arguments (uses only the cmd field)
uint8_t switchCommand(uint8_t cmd, uint8_t onOff); // Wrapper for quick use of onOff commands

#endif /* INC_TER_COMMAND_H_ */