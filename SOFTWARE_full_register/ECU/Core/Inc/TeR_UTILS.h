/*
 * TeR_UTILS.h
 *
 *      Author: Nguyen Nhan
 */
// This file holds diverse code utils that dont fall under any category

#ifndef INC_TER_UTILS_H_
#define INC_TER_UTILS_H_

#include "stm32f4xx.h"  // For bare-metal register access (e.g., SysTick)

// Boolean persistence checker (a function that checks if an error occurs for a certain time)
typedef uint32_t persist_t; // Stores the rising edge timestamp

// Function prototypes
uint8_t checkPersistance(persist_t *instance, uint8_t ok, uint32_t tMax);

// MAP (Mapping)
int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min,
		int32_t out_max); // Very useful map function

#endif /* INC_TER_UTILS_H_ */