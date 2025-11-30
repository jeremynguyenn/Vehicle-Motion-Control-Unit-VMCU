/*
 * torqueManager.h
 *
 *  Created on: Mar 30, 2024
 *      Author: Ozuba
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
Speed(Rpm)        │     Power       │  Torque    │     Driving      │           │      Traction     │
─────────────────►│     Limiter     ├───────────►│     Modes        │           │      Control      │
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

#ifndef INC_TER_TRQMANAGER_H_
#define INC_TER_TRQMANAGER_H_

#include <stdint.h>
#include "TeR_CAN.h"  // For controlling TeR vehicle
#include "pid.h"  // For torque vectoring

// Dynamic value types
typedef int32_t trq_t; // Signed for now due to regen/reverse, makes sense (sanity checks will be implemented)
typedef struct { // If you want a 4WD, add 2 more members and go
	trq_t rLeft; // Rear left wheel
	trq_t rRight; // Rear right wheel
} trqMap_t;

// Manager configs
typedef struct { // Contains pipeline configurations
	trq_t (*limiter)(void); // Takes a power limitation value in kW and returns the developable torque (trqLimit)
	trqMap_t (*drivingMode)(trq_t trqLimit); // Takes a limit torque and distributes it according to the mode on the wheels
	trqMap_t (*tractionControl)(trqMap_t trqMap); // Traction control function pointer
} trqPipeline_t;

// Expose to other modules the pipeline configuration (can only be changed outside driving via command system)
extern trqPipeline_t DriveConfig;

// Main functions
uint8_t trqManager(void); // Executes all the torque pipeline

// Basic limiters
trq_t limitTorque(void); // Constant maximum torque
trq_t limitMechPWR(void); // Constant mechanical power
trq_t limitSpeed(void); // Maximum speed limit
trq_t limitElecPWR(void); // Limit electrical power

// Basic modes
trqMap_t lineal(trq_t limit); // Linear torque delivery to both wheels

// Basic traction Control
trqMap_t tractionControlOFF(trqMap_t in); // Traction control off

#endif /* INC_TER_TRQMANAGER_H_ */