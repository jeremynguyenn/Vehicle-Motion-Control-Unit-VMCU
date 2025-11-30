/*
 * TeR_UTILS.c
 *
 *      Author: Nguyen Nhan
 * 
 * Utility functions for persistence checking and value mapping.
 */
#include "TeR_UTILS.h"
#include "stm32f4xx.h"  // For SysTick access

// Checks if an error persists for more than tMax milliseconds
uint8_t checkPersistance(persist_t *instance, uint8_t ok, uint32_t tMax) {

	if (*instance > 0) { // We were in error state
		if (ok) { // No error now
			*instance = 0; // Reset timestamp to 0, no error
		} else if (SysTick->VAL - *instance >= tMax * (SystemCoreClock / 1000)) { // Error exceeds max time (using SysTick for timing)
			return 0; // Return error
		}
	} else if (!ok) { // Was not in error, now is
		*instance = SysTick->VAL;  // Set current SysTick value as timestamp
	}

	return 1; // Error exists but has not exceeded maxTime
}


// Maps a value from one interval to another
int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min,
		int32_t out_max) {
// Saturate outputs if input exceeds calibration limits
	if (x < in_min)
		return out_min;  // Return minimum output if below input min
	if (x > in_max)
		return out_max;  // Return maximum output if above input max
// Map if within safe range
	long val = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;  // Linear interpolation formula
	return val;  // Return mapped value
}