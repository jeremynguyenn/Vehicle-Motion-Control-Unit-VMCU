/*
 * TeR_REFRI.c
 *
 *      Author: Nguyen Nhan
 * 
 * This file manages the refrigeration system for the inverters and motors.
 * It checks temperatures and toggles the refrigeration relay accordingly.
 */

#include "TeR_REFRI.h"

// Global persistence structure for refrigeration state tracking
persist_t REFRI;

// Function to manage refrigeration based on temperature thresholds
void refriManager() {
	// Gather inverter IGBT temperatures in degrees Celsius
	int16_t rInvTemp = inverter_emcu_state_4_right_pwr_stg_temp_deg_c_decode(
			TeR.tempsRight.pwr_stg_temp_deg_c);  // Decode right inverter power stage temperature
	int16_t lInvTemp = inverter_emcu_state_4_left_pwr_stg_temp_deg_c_decode(
			TeR.tempsLeft.pwr_stg_temp_deg_c);   // Decode left inverter power stage temperature

	// Gather motor temperatures in degrees Celsius
	int16_t rMotorTemp =
			inverter_emcu_state_4_right_e_machine_temp_2_deg_c_decode(
					TeR.tempsRight.e_machine_temp_2_deg_c);  // Decode right motor temperature
	int16_t lMotorTemp =
			inverter_emcu_state_4_left_e_machine_temp_2_deg_c_decode(
					TeR.tempsLeft.e_machine_temp_2_deg_c);   // Decode left motor temperature
	
	// Check if any temperature exceeds the ON threshold and refrigeration is off
	if (((rInvTemp > ON_THRESHOLD || lInvTemp > ON_THRESHOLD)
			|| (rMotorTemp > ON_THRESHOLD || lMotorTemp > ON_THRESHOLD))
					&& !TeR.status.refri) {
		// Turn on refrigeration
		switchCommand(TER_COMMAND_CMD_SWITCH_REFRI_CHOICE,
		TER_COMMAND_ONOFF_ON_CHOICE);
	} else if ((rInvTemp < OFF_THRESHOLD) && (lInvTemp < OFF_THRESHOLD)
			&& (rMotorTemp < OFF_THRESHOLD) && (lMotorTemp < OFF_THRESHOLD)
			&& TeR.status.refri) {
		// Turn off refrigeration
		switchCommand(TER_COMMAND_CMD_SWITCH_REFRI_CHOICE,
		TER_COMMAND_ONOFF_OFF_CHOICE);
	}
}