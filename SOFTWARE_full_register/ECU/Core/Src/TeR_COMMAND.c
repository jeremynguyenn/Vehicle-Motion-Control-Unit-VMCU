/*
 * TeR_COMMAND.c
 *
 *      Author: Nguyen Nhan
 * 
 * Command handling functions.
 */
#include "TeR_COMMAND.h"
#include "stm32f4xx.h"  // For GPIO and delay

// Implement commands here
uint8_t command(struct ter_command_t command) {
	// Volatile buffers for sending
	uint8_t TxData[8]; // Data buffer for transmission
	CAN_TxHeaderTypeDef TxHeader; // Transmission header (struct for reference)
	uint32_t mailbox; // Temporary slot for message
	TxHeader.IDE = CAN_ID_STD;  // Standard ID
	TxHeader.RTR = CAN_RTR_DATA;// Data frame

	// Preinitialize response
	struct ter_response_t response;  // Response structure
	response.cmd = command.cmd;      // Set command ID
	response.code = TER_RESPONSE_CODE_OK_CHOICE; // Default to OK

	/*-----------------------------------------[COMMANDS]---------------------------------------*/
	switch (command.cmd) { // Generate a file with defines for this in DBCS repo

	case TER_COMMAND_CMD_PRECHARGE_CHOICE: // Precharge
		if (TeR.status.state == RDY2PRECH) { // Send precharge message to BMS
			TeR.BmsAppReq.app_state_req =
			HVBMS_BMS_RX_CTRL_1_APP_STATE_REQ_HV__PRECHARGE_CHOICE; // Request precharge from BMS
			TxHeader.StdId = HVBMS_BMS_RX_CTRL_1_FRAME_ID; // BMS precharge action ID
			TxHeader.DLC = HVBMS_BMS_RX_CTRL_1_LENGTH;     // Data length
			hvbms_bms_rx_ctrl_1_pack(TxData, &TeR.BmsAppReq, TxHeader.DLC); // Pack data
			// Bare-metal send (as in TeR_CAN.c)
			while ((mainCAN->TSR & CAN_TSR_TME0) == 0);  // Wait for mailbox
			mainCAN->sTxMailBox[0].TIR = (TxHeader.StdId << 21) | (TxHeader.IDE << 2) | (TxHeader.RTR << 1);
			mainCAN->sTxMailBox[0].TDTR = TxHeader.DLC;
			mainCAN->sTxMailBox[0].TDLR = *(uint32_t*)TxData;
			mainCAN->sTxMailBox[0].TDHR = *(uint32_t*)(TxData + 4);
			mainCAN->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
		} else {
			response.code = TER_RESPONSE_CODE_INVALID_STATE_CHOICE; // Invalid state error
		}
		break;

	case TER_COMMAND_CMD_DISCHARGE_CHOICE: // Discharge
		TeR.BmsAppReq.app_state_req =
		HVBMS_BMS_RX_CTRL_1_APP_STATE_REQ_HV__SHUTDOWN_CHOICE; // Ask for HV_Shutdown
		TxHeader.StdId = HVBMS_BMS_RX_CTRL_1_FRAME_ID; // BMS app_state_req
		TxHeader.DLC = HVBMS_BMS_RX_CTRL_1_LENGTH;
		hvbms_bms_rx_ctrl_1_pack(TxData, &TeR.BmsAppReq, TxHeader.DLC);
		// Send bare-metal
		while ((mainCAN->TSR & CAN_TSR_TME0) == 0);
		mainCAN->sTxMailBox[0].TIR = (TxHeader.StdId << 21) | (TxHeader.IDE << 2) | (TxHeader.RTR << 1);
		mainCAN->sTxMailBox[0].TDTR = TxHeader.DLC;
		mainCAN->sTxMailBox[0].TDLR = *(uint32_t*)TxData;
		mainCAN->sTxMailBox[0].TDHR = *(uint32_t*)(TxData + 4);
		mainCAN->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
		break;


	case TER_COMMAND_CMD_RESET_BMS_CHOICE: // Reset BMS
		TeR.BmsAppReq.app_state_req =
		HVBMS_BMS_RX_CTRL_1_APP_STATE_REQ_RESET_CHOICE; // Ask for HV_Reset
		TxHeader.StdId = HVBMS_BMS_RX_CTRL_1_FRAME_ID; // BMS app_state_req
		TxHeader.DLC = HVBMS_BMS_RX_CTRL_1_LENGTH;
		hvbms_bms_rx_ctrl_1_pack(TxData, &TeR.BmsAppReq, TxHeader.DLC);
		// Send bare-metal
		while ((mainCAN->TSR & CAN_TSR_TME0) == 0);
		mainCAN->sTxMailBox[0].TIR = (TxHeader.StdId << 21) | (TxHeader.IDE << 2) | (TxHeader.RTR << 1);
		mainCAN->sTxMailBox[0].TDTR = TxHeader.DLC;
		mainCAN->sTxMailBox[0].TDLR = *(uint32_t*)TxData;
		mainCAN->sTxMailBox[0].TDHR = *(uint32_t*)(TxData + 4);
		mainCAN->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
		break;


	case TER_COMMAND_CMD_READY2_DRIVE_CHOICE: // Ready2Drive
		if (TeR.status.state == PRECHARGED && TeR.bpps.bpps > 4) { // Put the car in driving mode and add brake

		// Allow transition to drive state
			TeR.status.r2_d = 1;
			TeR.appReqRight.app_state_req = 4;
			TeR.appReqLeft.app_state_req = 4;
		} else {
			response.code = TER_RESPONSE_CODE_INVALID_STATE_CHOICE;
		}
		break;

	case TER_COMMAND_CMD_BEEP_CHOICE: // MADAFUKIN BEEP
		GPIOA->BSRR = GPIO_PIN_12; // Set horn pin
		delay_ms(500);
		GPIOA->BSRR = GPIO_PIN_12 << 16; // Reset horn pin
		break;

	case TER_COMMAND_CMD_SET_DYNAMIC_CONFIG_CHOICE:
		trqPipeline_t conf;
		if (TeR.status.state < DRIVING) {
			// Limiters
			switch (command.cfg_limiter) {
			case TER_DYNAMIC_CONFIG_LIMITER_LIMIT_TORQUE_CHOICE:
				conf.limiter = &limitTorque; // Configure limiter to torque
				break;

			case TER_DYNAMIC_CONFIG_LIMITER_LIMIT_MECH_PWR_CHOICE:
				conf.limiter = &limitMechPWR; // Configure limiter to mechanical power
				break;

			default:
				response.code = TER_RESPONSE_CODE_OUT_OF_RANGE_CHOICE;
				break;
			}
			switch (command.cfg_mode) {			// Mode

			case TER_DYNAMIC_CONFIG_MODE_LINEAL_CHOICE:
				conf.drivingMode = &lineal; // Configure mode to linear
				break;

			default:
				response.code = TER_RESPONSE_CODE_OUT_OF_RANGE_CHOICE;
				break;

			}
			switch (command.cfg_traction_control) {			// Traction Control

			case TER_DYNAMIC_CONFIG_TRACTION_CONTROL_OFF_CHOICE:
				conf.tractionControl = &tractionControlOFF; // Configure traction control off
				break;

			default:
				response.code = TER_RESPONSE_CODE_OUT_OF_RANGE_CHOICE;
				break;

			}
		} else {
			response.code = TER_RESPONSE_CODE_INVALID_STATE_CHOICE;
		}
		if (response.code == TER_RESPONSE_CODE_OK_CHOICE) { // Was in range
			TeR.dynamicConfig.limiter = command.cfg_limiter;
			TeR.dynamicConfig.mode = command.cfg_mode;
			TeR.dynamicConfig.traction_control = command.cfg_traction_control;
			DriveConfig = conf; // Copy to pipeline struct only if OK
		}

		break;

	case TER_COMMAND_CMD_SET_LIMITS_CHOICE:
		if (ter_command_trq_limit_is_in_range(command.trq_limit)) {
			TeR.dynamicConfig.trq_limit = command.trq_limit;
		}
		if (ter_command_kw_limit_is_in_range(command.kw_limit)) {
			TeR.dynamicConfig.kw_limit = command.kw_limit;
		}
		if (ter_command_speed_limit_is_in_range(command.speed_limit)) {
			TeR.dynamicConfig.speed_limit = command.speed_limit;
		}
		break;

	case TER_COMMAND_CMD_SWITCH_SCS_CHOICE:
		if (command.onoff) {
			startSCS();
		} else { // if disabled enable
			stopSCS();
		}
		break;

		/*Sends messages not implemented in this board to the main can if the source is internal*/
	default: // Handles commands not implemented here
		if (!NVIC_GetActive(CAN2_RX0_IRQn)) { // Checks if command is being attended from an external source (CAN2)
			TxHeader.StdId = TER_COMMAND_FRAME_ID;
			TxHeader.DLC = TER_COMMAND_LENGTH;
			ter_command_pack(TxData, &command, TER_COMMAND_LENGTH);
			while((mainCAN->TSR & CAN_TSR_TME0) == 0);
			mainCAN->sTxMailBox[0].TIR = (TxHeader.StdId << 21) | (TxHeader.IDE << 2) | (TxHeader.RTR << 1);
			mainCAN->sTxMailBox[0].TDTR = TxHeader.DLC;
			mainCAN->sTxMailBox[0].TDLR = *(uint32_t*)TxData;
			mainCAN->sTxMailBox[0].TDHR = *(uint32_t*)(TxData + 4);
			mainCAN->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
			return 0; // Exit function, no result
		}
		break;

	}
	/* Return a response message */
	TxHeader.StdId = TER_RESPONSE_FRAME_ID;
	TxHeader.DLC = TER_RESPONSE_LENGTH;
	ter_response_pack(TxData, &response, TxHeader.DLC);
	// Send bare-metal
	while ((mainCAN->TSR & CAN_TSR_TME0) == 0);
	mainCAN->sTxMailBox[0].TIR = (TxHeader.StdId << 21) | (TxHeader.IDE << 2) | (TxHeader.RTR << 1);
	mainCAN->sTxMailBox[0].TDTR = TxHeader.DLC;
	mainCAN->sTxMailBox[0].TDLR = *(uint32_t*)TxData;
	mainCAN->sTxMailBox[0].TDHR = *(uint32_t*)(TxData + 4);
	mainCAN->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
	return 1;
}

uint8_t easyCommand(uint8_t cmd) {
	struct ter_command_t cmdMsg;
	ter_command_init(&cmdMsg);
	cmdMsg.cmd = cmd;
	return command(cmdMsg);
}

uint8_t switchCommand(uint8_t cmd, uint8_t onOff) {
	struct ter_command_t cmdMsg;
	ter_command_init(&cmdMsg);
	cmdMsg.cmd = cmd;
	cmdMsg.onoff = onOff;
	return command(cmdMsg);
}