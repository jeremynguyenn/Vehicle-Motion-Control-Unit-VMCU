/*
 * pid.c
 *
 *      Author: Nguyen Nhan
 *
 * A simple PID library using floating arithmetic for its usage all along the control systems
 * of the car. (Torque Vectoring, Thermal Management...). It is thought to be running in a periodic interrupt
 * but it can be easily modified to perceive time using a timeBase timer..
 *
 *
 * -------------------------------------------------[Safety]--------------------------------------------
 * This is a controller, someone is riding on top, if you mess up, it's dangerous. This library
 * Implements safety measures such as the possibility to saturate individually the Integral and Derivative terms
 */
#include "pid.h"

// Initialize PID with gains and limits
pid_t* initPID(float Kp, float Ki, float Kd, float loopTime, float iMax) {
	pid_t *pid; // Temporary pointer for init
	pid = (pid_t*) calloc(1, sizeof(pid)); // Allocates one PID instance and zeros memory to prevent issues
	pid->Kp = Kp;  // Set proportional gain
	pid->Ki = Ki;  // Set integral gain
	pid->Kd = Kd;  // Set derivative gain
	pid->T = loopTime; // Loop time
	pid->antiWindup = iMax; // Saturation value for integral term
	return pid;  // Return pointer to PID structure
}

// Deinitialize PID
void deInitPID(pid_t *pid) {
	free(pid); // Free memory
}

// Compute PID output
float pid(pid_t *pid, float ref, float feedback) {
// PID calculations
	pid->error = (ref - feedback); // Proportional error
	pid->errorI += pid->error * pid->T; // Integral accumulation
	pid->errorD = (pid->error - pid->prevError) / pid->T; // Derivative
// Anti-windup check
	pid->errorI =
			(pid->errorI < pid->antiWindup) ? pid->errorI : +pid->antiWindup; // Upper bound check
	pid->errorI =
			(pid->errorI > -pid->antiWindup) ? pid->errorI : -pid->antiWindup; // Lower bound check

	return pid->Kp * pid->error + pid->Ki * pid->errorI + pid->Kd * pid->errorD; // Return P + I + D
}