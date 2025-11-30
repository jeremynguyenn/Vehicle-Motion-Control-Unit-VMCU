/*
 * TeR_REFRI.h
 *
 *      Author: Nguyen  Nhan
 */
// File in charge of managing the cooling system
// Implements a hysteresis controller for now

#ifndef INC_TER_REFRI_H_
#define INC_TER_REFRI_H_

#include "TeR_UTILS.h"  // For persistence
#include "TeR_CAN.h"  // For TeR structures
#include "TeR_COMMAND.h"  // For switch commands

// Thresholds
#define ON_THRESHOLD 50 // Turns on at 50 degrees
#define OFF_THRESHOLD 40 // Turns off at 40 degrees

// Function prototypes
void refriManager();  // Executes the cooling loop

#endif /* INC_TER_REFRI_H_ */