/*
 * tv_mds.h
 *
 *      Author: Nguyen Nhan
 */

#ifndef INC_TV_MDS_H_
#define INC_TV_MDS_H_

#include "TeR_CAN.h"  // For TeR structures
#include "pid.h"  // For PID structure

// Constants
#define DEG2RAD 0.0174533 // Degrees to radians

/////////////////////////////////////////[Vehicle Constants]/////////////////////////////////////////////////////////////

#define I_ZZ 122.0 // Moment of inertia on Z axis (kg*m^4) (Juan Gastaminza model)
#define T_WIDTH 1.185 // Track width REAR (m)
#define L_FRONT 0.806 // A Distance (from front axle to CG) (m)
#define L_REAR 0.744 // B Distance (from CG to rear axle) (m)
#define H_CDG 0.27   // Height of the CG (static) (m)
#define GEAR_R 5     // Reduction index
#define R_WHEEL 0.2023 // Wheel radius (m)

#define K_U 0 // Target understeer gradient (rad)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Function prototypes
float yawRef(float steer, float vx); // Function that takes wheel angle and forward speed and returns yaw rate reference
float mz2DeltaTorque(float alpha);  // Converts moment to delta torque
float trqDistribution();  // Torque distribution

#endif /* INC_TV_MDS_H_ */