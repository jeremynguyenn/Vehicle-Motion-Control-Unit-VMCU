/*
 * stateMachine.c
 *
 *      Author: Nguyen Nhan
 *
 * This file encapsulates the state machine of the TER:
 * It consists of 4 states:
 *-------------------------------------------------------------------------------------
 * WAITING_FOR_SL -> 0
 * - Initial state, checks if the safety loop is closed
 * - by reading the voltage value after the TSMS
 *
 * RDY2PRECH
 * - The safety loop is closed, precharging can be done
 *
 * PRECHARGING
 * - Transitional state until the BMS finishes precharging
 *
 * PRECHARGED
 * - The car is precharged, ready to drive (R2D) is allowed
 *
 * DRIVING
 * - The car can be driven
 *-------------------------------------------------------------------------------------
 * After evaluating different ways to implement the state machine,
 * the most optimal is to check the state each cycle in a privilege ladder
 * since the conditions of more complex states are contained in the higher ones,
 * this way if a condition is not met, it degrades to the lowest state.
 *
 */
#include "TeR_STATEMACHINE.h"
#include "stm32f4xx.h"  // Include for direct register access

// Pointer to the timer instance for the state machine
TIM_TypeDef *beat;

// Global persistence structure for safety loop
persist_t SL;

// Initialize the state machine with a timer
uint8_t initStateMachine(TIM_TypeDef *htim) {
	beat = htim; // Configure the state machine timer
	beat->DIER |= TIM_IT_UPDATE;  // Enable update interrupt on the timer
	beat->CR1 |= TIM_CR1_CEN;     // Start the timer
	return 1;                     // Return success
}

// Function to get the current state based on conditions
state_t getState(void) {
	state_t status = WAIT_SL; // Start with the initial state 0
	// Readings

	TeR.status.sl = checkPersistance(&SL,
			((GPIOB->IDR & TSMS_Pin) != 0), 500);// Read the safety loop state (1 if pin is high)
	TeR.status.bspd = ((GPIOB->IDR & GPIO_PIN_12) != 0);	// Read the BSPD state (1 if pin is high)

	if (TeR.status.sl) { // If safety loop is okay
		status = RDY2PRECH; // Can precharge
		if (TeR.BmsAppState.app_state_app
				== HVBMS_BMS_TX_STATE_3_APP_STATE_APP_HV__PRECHARGE_CHOICE) { // Is precharging happening?
			status = PRECHARGING;
		} else if (TeR.BmsAppState.app_state_app
				== HVBMS_BMS_TX_STATE_3_APP_STATE_APP_HV__READY_CHOICE) { // Is it precharged?
			status = PRECHARGED;
			if (TeR.status.r2_d
					&& ((TeR.appStateRight.app_state_app == 4)
							|| (TeR.appStateLeft.app_state_app == 4))) { // Ready-to-drive flag active and both inverters operational
				status = DRIVING;
			}
		}
	}
	return status; // Return the determined state
}

// State machine main function, called on timer interrupt
void stateMachine(TIM_TypeDef *beat) {
	uint8_t prevState = TeR.status.state; // Save the previous state
	TeR.status.state = getState(); // Get the current state
	uint8_t stateChanged = TeR.status.state != prevState ? 1 : 0; // Check if state changed for setup
	permaTask(); // Execute permanent tasks
	//-----------------------------------[Setups]--------------------------------------------//

	if (stateChanged) { // Handles setup conditions for the new state
		switch (TeR.status.state) {
		case WAIT_SL:
			// Security measures
			TeR.trqReqLeft.torque_nm_req = 0;  // Set left torque request to 0
			TeR.trqReqRight.torque_nm_req = 0; // Set right torque request to 0
			switchCommand(TER_COMMAND_CMD_SWITCH_REFRI_CHOICE,
					TER_COMMAND_ONOFF_OFF_CHOICE); // Turn off refrigeration
			easyCommand(TER_COMMAND_CMD_RESET_BMS_CHOICE); // Reset the BMS
			break;

		case RDY2PRECH:
			// Security measures
			TeR.trqReqLeft.torque_nm_req = 0; // Set left torque to 0 for safety
			TeR.trqReqRight.torque_nm_req = 0; // Set right torque to 0 for safety
			TeR.dynamicConfig.traction_control = TER_DYNAMIC_CONFIG_TRACTION_CONTROL_OFF_CHOICE; // Disable traction control in dynamic config
			// Call command to interpret and apply (assuming cmdMsg is prepared elsewhere or here)
			struct ter_command_t cmdMsg; // Command message structure
			ter_command_init(&cmdMsg); // Initialize command message
			command(cmdMsg); // Call the command function with initialized message
			break;
		case DRIVING:
			GPIOA->BSRR = GPIO_PIN_12;  // Set GPIOA Pin 12 high (e.g., for signal)
			delay_ms(2000);             // Delay for 2000 ms (EV 4.12.1 compliance)
			GPIOA->BSRR = GPIO_PIN_12 << 16;  // Reset GPIOA Pin 12 low
			startSCS(); // Start critical signals

			break;
		default:
			// Handle invalid state (e.g., log error or reset)
			break;
		}
	}

//-----------------------------------[LOOPS]--------------------------------------------//

	switch (TeR.status.state) {
	case WAIT_SL:
		waitSL(); // Check if safety loop is closed
		break;

	case RDY2PRECH:
		rdy2Prech(); // Wait for precharge command
		break;

	case PRECHARGING:
		precharging(); // Transitional state, monitor progress
		break;

	case PRECHARGED:
		precharged(); // Wait for ready-to-drive command
		break;
	case DRIVING:
		driving(); // Execute driving mode
		break;
	default:
			// Handle invalid state
		break;
	}

}

/* -------------------------[States]---------------------------- */

// Function for WAIT_SL state
void waitSL(void) {

} // Checks if the safety loop is closed

// Function for RDY2PRECH state
void rdy2Prech(void) {

} // Waits to receive the precharge command

// Function for PRECHARGING state
void precharging(void) {

} // Transitional state, monitors that everything is going well

// Function for PRECHARGED state
void precharged(void) {

} // Waits to receive the ready-to-drive command

// Function for DRIVING state
void driving(void) {
	trqManager(); // Executes the torque pipeline

} // Executes the torque command

/* -------------------------[Permanent Tasks]---------------------------- */

// Function for tasks that run in every state
void permaTask() {
	// Refrigeration Management (commented out in original)
	//refriManager();

// Brake Light control
	if (TeR.bpps.bpps > 4) {
		GPIOB->BSRR = BL_Pin;  // Turn on brake light if brake pressure > 4
	} else {
		GPIOB->BSRR = BL_Pin << 16;  // Turn off brake light
	}
// Process Wheel Data
	TeR.wheelInfo.rl_rpm = ((-TeR.dqErpmLeft.e_machine_speed_erpm) / MOTOR_POLES)
			* RED_RATIO;  // Calculate left rear wheel RPM
	TeR.wheelInfo.rr_rpm = (TeR.dqErpmRight.e_machine_speed_erpm / MOTOR_POLES)
			* RED_RATIO;  // Calculate right rear wheel RPM
	TeR.wheelInfo.rl_trq = TeR.trqEstLeft.torque_est_nm / RED_RATIO;  // Calculate left rear torque
	TeR.wheelInfo.rr_trq = TeR.trqEstRight.torque_est_nm / RED_RATIO; // Calculate right rear torque
	TeR.wheelInfo.speed = 3.6*(TeR.wheelInfo.rl_rpm * 2 * PI * WHEEL_RADIUS) / 60; // Calculate linear vehicle speed in km/h

// Bypass Inverter data
	TeR.invInfo.left_dem = TeR.demLeft.dem; // Copy left DEM value
	TeR.invInfo.right_dem = TeR.demRight.dem; // Copy right DEM value

	TeR.invInfo.left_motor_temp = (uint8_t)inverter_emcu_state_4_left_e_machine_temp_2_deg_c_decode(
			TeR.tempsLeft.e_machine_temp_2_deg_c);  // Decode and cast left motor temperature
	TeR.invInfo.right_motor_temp = (uint8_t)inverter_emcu_state_4_right_e_machine_temp_2_deg_c_decode(
			TeR.tempsRight.e_machine_temp_2_deg_c); // Decode and cast right motor temperature

	TeR.invInfo.left_power_stage_temp = (uint8_t)inverter_emcu_state_4_left_pwr_stg_temp_deg_c_decode(
			TeR.tempsLeft.pwr_stg_temp_deg_c);  // Decode and cast left power stage temperature

	TeR.invInfo.right_power_stage_temp = (uint8_t)inverter_emcu_state_4_right_pwr_stg_temp_deg_c_decode(
			TeR.tempsRight.pwr_stg_temp_deg_c); // Decode and cast right power stage temperature

// Fill in Status Message
	TeR.status.ams = TeR.BmsAppState.dio1_state; // Set AMS status (1 = OK)
	TeR.status.imd = TeR.BmsAppState.dio2_state; // Set IMD status (1 = OK)
	TeR.status.left_inv = (TeR.appStateLeft.app_state_app != 6); // Check if left inverter is not in fault state
	TeR.status.right_inv = (TeR.appStateRight.app_state_app != 6); // Check if right inverter is not in fault state
	TeR.status.refri = TeR.lvbms.refri_on; // Set refrigeration relay status

// Check System Critical Signals
	checkSCS();

}

// Custom delay function using a simple loop (bare-metal, approximate)
void delay_ms(uint32_t ms) {
    volatile uint32_t count = ms * (SystemCoreClock / 1000 / 8);  // Calculate loop count based on system clock
    while (count--) {  // Decrement until zero
        __NOP();  // No operation (prevent optimization)
    }
}