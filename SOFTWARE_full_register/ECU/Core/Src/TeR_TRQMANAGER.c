/*
 * torqueManager.c
 *
 *      Author: Nguyen Nhan
 *
 */
/*
                                             STEER
                                             ────────────────────┐
                                             APPS                │
             KWLimit                         ───────────────┐    │
             ──────────────┐                 IMU            │    │                                                      ┌────────┐
                           │                 ──────────┐    │    │                                              ┌──────►│RIGHT   │
                           ▼                           ▼    ▼    ▼                                              │       │INVERTER│
                  ┌──────────────────┐            ┌──────────────────┐ TorqueR   ┌───────────────────┐ TorqueR  │       └────────┘
                  │                  │            │                  ├──────────►│                   ├──────────┘
Speed(Rpm)        │     Power        │  Torque    │     Driving      │           │      Traction     │
─────────────────►│     Limiter      ├───────────►│     Modes        │           │      Control      │
                  │                  │ Available  │                  │ TorqueL   │                   │ TorqueL
                  │                  │            │                  ├──────────►│                   ├──────────┐
                  └──────────────────┘            └──────────────────┘           └───────────────────┘          │
                                                                                                                │       ┌────────┐
                                                                                                                └──────►│LEFT    │
                                                                                                                        │INVERTER│
                                                                                                                        └────────┘

 The torque manager is the library in charge of managing the dynamic control of the vehicle, it consists on 3 differentiated stages
 which are modular and interchangeable with the idea of creating different driving experiences according to the competition. The principal
 interchangeable block is the driving mode


 - Power Limiter: Sets a power limitation in kW and calculates maximum developable torque assuming power
   is conserved along the powertrain Pelectrica = Pmecanica*EfficiencyFactor

 - Driving Mode:
 	 * Linear: Torque is distributed equally to the 2 wheels
 	 * BasicTorque: Torque distribution based on a steering function (usually a polynomial)
 	 * ControlTorque: Torque distribution based on a scheduled gain PID calibrated using a bicycle model of the vehicle
 	 * Reverse Mode: Self-explanatory (will beep in work mode haha) Very illegal in competition
	 * Autonomous (Future): Allows control of the TeR using the TeR_COMMAND library
 - Traction Control:
 	 * Feedforward
 	 * Feedback etc


To allow modularity, a processing cycle based on function pointers is used, allowing easy writing of functions in other modules
- Power limiter void -> trq_t (Estimation is internal to the module, not entered as argument (can make estimates based on electrical consumption))
- Driving mode trq_t -> trqMap_t
- Traction Control trqMap_t -> trqMap_t
 */
#include "TeR_TRQMANAGER.h"

// Current configuration in use
trqPipeline_t DriveConfig; // Configuration in use

// Main torque manager function: runs the pipeline and requests command
uint8_t trqManager(void) { // Runs the pipeline stages and requests the command
	// Pipeline
	trq_t availableTorque = DriveConfig.limiter(); // Limit torque/power
	trqMap_t trqDistribution = DriveConfig.drivingMode(availableTorque); // Distribute torque
	trqMap_t trqToWheels = DriveConfig.tractionControl(trqDistribution); // Limit if wheel can't grip (strategy dependent on controller)

	// Command request
	TeR.trqReqLeft.torque_nm_req = trqToWheels.rLeft;  // Set left torque request
	TeR.trqReqRight.torque_nm_req = trqToWheels.rRight; // Set right torque request
	// Safety checks and saturations: (Redundant but allow peace of mind)

	if (TeR.trqReqLeft.torque_nm_req < 0 || TeR.trqReqLeft.torque_nm_req < 0) {  // Check if any torque is negative
		TeR.trqReqLeft.torque_nm_req = 0;  // Set to 0 for safety
		TeR.trqReqRight.torque_nm_req = 0; // Set to 0 for safety
	}

	return 1;  // Return success
}


//------------------------------------------------[Basic Power Limiters]------------------------------------------------//
// void -> trq_t
// Constant maximum torque
trq_t limitTorque(void) {
	return (trq_t) TeR.dynamicConfig.trq_limit; // Return the configured value
}

// Constant mechanical power
trq_t limitMechPWR(void) {
	int32_t meanRPM = (TeR.wheelInfo.rl_rpm + TeR.wheelInfo.rl_rpm)
			/ 2; // Average RPMs
	if (meanRPM > 0) { // Moving, estimate developable torque
		return (trq_t) (TeR.dynamicConfig.kw_limit * ELEC2MECH_EFF) / meanRPM; // Return developable power limited by power
	} else if (meanRPM == 0) { // Stationary, return maximum torque limit
		return (trq_t) TeR.dynamicConfig.trq_limit;
	}
	return 0;  // Default return 0
}

// Maximum speed limit
trq_t limitSpeed(void) {
// Reduces torque as approaching speed value (may cause oscillatory issues)
// Speed increases -> torque decreases -> Speed decreases -> Torque increases
// Use PID
	return 0;  // Placeholder return
}

// Limit electrical power (from Inverters Controller data)
trq_t limitElecPWR(void) {
	return 0;  // Placeholder return
}

//------------------------------------------------[Basic Driving Modes]------------------------------------------------//
// trq_t -> trqMap_t
// Linear torque delivery to both wheels
trqMap_t lineal(trq_t limit) { 
	trqMap_t trqMap;  // Torque map structure
	trqMap.rLeft = map(TeR.apps.apps_av, 0, 255, 0, limit*0.5);   // Map left torque
	trqMap.rRight = map(TeR.apps.apps_av, 0, 255, 0, limit*0.5);  // Map right torque
	return trqMap;  // Return torque map
}

//------------------------------------------------[Basic traction Control]------------------------------------------------//
// trqMap_t -> trqMap_t
// Traction control off: pass through input
trqMap_t tractionControlOFF(trqMap_t in) {
return in;  // Return input unchanged
}