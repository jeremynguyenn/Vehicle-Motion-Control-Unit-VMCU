/*
 * TeR_CONSTANTS.h
 *
 *      Author: Nguyen Nhan
 */

#ifndef INC_TER_CONSTANTS_H_
#define INC_TER_CONSTANTS_H_


// GENERAL
#define PI 3.14159265358979323846


// INVERTER RELATED
#define MOTOR_POLES 10 // For erpm to rpm or speed calculation


// TER RELATED
#define WHEEL_RADIUS 0.2032 // in m
#define RED_RATIO 1.0/5.0 // Gear ratio for torque/speed conversion with the motor
#define ELEC2MECH_EFF 0.94 // Estimation of electrical-mechanical efficiency

#endif /* INC_TER_CONSTANTS_H_ */