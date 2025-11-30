/*
 * scs.c
 *
 *      Author: Nguyen Nhan
 * 
 * This file handles System Critical Signals (SCS) monitoring.
 * Add the check callback to your favorite interrupt manager.
 */

#include "TeR_SCS.h"
#include "stm32f4xx.h"  // Include for register access

/*
 * You need to add the check callback to your favorite interrupt manager.
 */

// Internal ID and timestamp lists
uint32_t scsIds[] = SCS; // The signals are added by the preprocessor macro
const uint8_t nSCS = sizeof(scsIds)/sizeof(uint32_t);  // Calculate number of SCS IDs
uint32_t timestamps[sizeof(scsIds)/sizeof(uint32_t)];  // Array for timestamps

uint32_t lastFailSCS; // ID of the last failed signal (for debugging)


// Timer pointer used by the module
TIM_TypeDef *base;


// Initialize SCS with a timer base
uint8_t initSCS(TIM_TypeDef *timBase) {
	base = timBase;  // Store the timer base
	startSCS();      // Activate SCS checking
	return 1;        // Return success
}

// Start SCS monitoring
uint8_t startSCS(void) { // Activate active time checking
	// Reset timer and timestamps to avoid errors on restart
	memset(&timestamps,0,sizeof(timestamps));  // Clear timestamps array
	base->CNT  = 0;                            // Reset timer counter
	// Start the timer
	base->CR1 |= TIM_CR1_CEN; // Start our time base (note: will overflow in 52 days)
	TeR.status.scs = 1;       // Set SCS status to active
	return 1;                 // Return success
}

// Stop SCS monitoring
uint8_t stopSCS(void) { // Deactivate active time checking
	base->CR1 &= ~TIM_CR1_CEN; // Freeze the timer so checks differ by 0 from now on
	TeR.status.scs = 0;        // Set SCS status to inactive
	return 1;                  // Return success
}

// Log SCS event
uint8_t logSCS(uint32_t id) {
	uint8_t i = 0;  // Index for searching
	while(i < nSCS && scsIds[i] != id) // Loop until match or end of array (short-circuit evaluation prevents out-of-bounds)
		 i++; // Check if ID is in the critical signals list, stop on match or exceed array size
	if (i >= nSCS) { // Traversed entire array (i is not a valid index N-1)
		return 0;    // Not an SCS
	} else {         // It was an SCS
		timestamps[i] = base->CNT; // Log the counter value in its slot
		return 1;    // It is an SCS
	}
	return -1;       // Should never reach here
}

// Check SCS for timeouts
void checkSCS(void) {
	for (uint8_t i = 0; i < nSCS; i++) {  // Loop through all SCS
		if (base->CNT - timestamps[i] > SCS_TIMEOUT) { // Check if timeout occurred
			lastFailSCS  = scsIds[i]; // Save the ID of the problematic signal for debug
			//easyCommand(TER_COMMAND_CMD_DISCHARGE_CHOICE); // Discharge the car (commented)
			TeR.apps.apps_av = 0; // Safety measure: set apps average to 0
		}
	}
}