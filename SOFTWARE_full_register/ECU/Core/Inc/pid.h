/*
 * pid.h
 *
 *      Author: Nguyen Nhan
 * A simple PID library using floating arithmetic for its usage all along the control systems
 * of the car.
 */

#ifndef INC_PID_H_
#define INC_PID_H_

#include <stdlib.h> // For memory allocation

// PID structure
typedef struct {
	// State Variables
	float error; // Proportional error
	float errorD; // Derivative error
	float errorI; // Integral error

	// Config Variables
	float Kp;  // Proportional gain
	float Ki;   // Integral gain
	float Kd;  // Derivative gain

	float prevError; // Previous error for derivative
	float antiWindup; // Saturation value for integral term (can be understood in integral units, max angular velocity for yawRate)
	float T; // Loop time
} pid_t;

// Function prototypes
pid_t* initPID(float Kp, float Ki, float Kd, float loopTime, float iMax); // Returns a PID instance in the HEAP
void deInitPID(pid_t* pid); // Frees PID memory
float pid(pid_t *pid, float ref, float feedback); // Calculates PID response

#endif /* INC_PID_H_ */