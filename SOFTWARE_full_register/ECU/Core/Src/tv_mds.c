/*
 * tv_mds.c
 *
 *      Author: Nguyen Nhan
 * 
 * Torque vectoring and distribution functions using PID.
 */

#include "tv_mds.h"

// Global PID structure for torque vectoring
pid_t tvPid; // PID structure

// Calculate yaw reference based on steering and velocity
float yawRef(float steer, float vx) {
	return (steer * vx) / ((L_FRONT + L_REAR) + K_U * (vx * vx));  // Yaw rate formula using vehicle parameters
}

// Convert moment to delta torque
float mz2DeltaTorque(float alpha) { // Takes PID output (review output units)
	return I_ZZ * (2 * R_WHEEL) / (GEAR_R * T_WIDTH) * alpha;  // Torque calculation using vehicle constants
}

// Torque distribution function
float trqDistribution() {
	float dTorque = 0;  // Initialize delta torque
	// Compute Torque
	float ref = yawRef(TeR.steer.angle, TeR.speed.vx_av);  // Get yaw reference
	float imuYawR = ter_ang_rate_yaw_rate_z_decode(TeR.angRate.yaw_rate_z);  // Decode IMU yaw rate
	float corr = pid(&tvPid, ref, imuYawR); // Compute PID correction
	dTorque = mz2DeltaTorque(corr);  // Convert to delta torque

	// Compute Torque output
	float gas = TeR.apps.apps_av / 255.0; // Gas command from 0-1

	TeR.trqReqRight.torque_nm_req = gas * 180 / 2 + dTorque / 2;  // Right torque request
	TeR.trqReqLeft.torque_nm_req = gas * 180 / 2 - dTorque / 2;   // Left torque request
	return 1;  // Return success
}