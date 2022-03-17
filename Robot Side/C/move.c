#include "motors.h"
#include "sensors/proximity.h"
#include "leds.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>


#define WHEEL_RADIUS 0.0205 // in m
#define AXLE_LENGTH 0.051 // in m
#define PI 3.141592654
#define PI2 6.283185307
#define MAX_VELOCITY 0.1292682914927  // in m/s


double find_speed(int speed) {
	if (speed > 1000) {
		speed = 1000;
	} else if (speed < -1000) {
		speed = -1000;
	}
	double new_speed = MAX_VELOCITY * speed / 1000;
	return new_speed;
}

double calculateF(int speed) {
	double T = AXLE_LENGTH * PI / find_speed(speed);
	return 1.0/T;
}

double calculateVrobot(int speed) {
	return find_speed(speed) * calculateF(speed);
}

double calculateS(double angle) {
	double angle_per_circle = 1.0 / 360.0 *angle;
	double s = AXLE_LENGTH * PI * angle_per_circle;
	return s;
}

double calculateTime(double angle, int speed) {
	double v = calculateVrobot(speed);
	double s = calculateS(angle);
	double bs = s/v;
	return bs;
}

void moveForward(int speed){
	left_motor_set_speed(speed);
	right_motor_set_speed(speed);
    chThdSleepMilliseconds(500);
}

void moveBackward(int speed){
	left_motor_set_speed(-speed);
	right_motor_set_speed(-speed);
    chThdSleepMilliseconds(500);
}

void turnRight(int speed) {
//	double time_turn = calculateTime(90, speed);
//	left_motor_set_speed(-speed);
//	right_motor_set_speed(speed);
//    chThdSleepMilliseconds(time_turn);
//	left_motor_set_speed(0);
//	right_motor_set_speed(0);
//    chThdSleepMilliseconds(1000);

	double new_speed = MAX_VELOCITY * speed / 1000;
	double T = 1.0 / (AXLE_LENGTH * PI / find_speed(speed));
	double angle_per_circle = 1.0 / 360.0 * 90;
	double s = AXLE_LENGTH * PI * angle_per_circle;
	double v = new_speed * T;
	double bs = s/v;
	double time_turn = calculateTime(1.0 * 90, 1.0 * speed);
	left_motor_set_speed(-speed);
	right_motor_set_speed(speed);
    chThdSleepMilliseconds(bs * 1000);

	left_motor_set_speed(0);
	right_motor_set_speed(0);
    chThdSleepMilliseconds(1000);

    moveForward(speed);
}

void turnLeft(int speed) {
	double new_speed = MAX_VELOCITY * speed / 1000;
	double T = 1.0 / (AXLE_LENGTH * PI / find_speed(speed));
	double angle_per_circle = 1.0 / 360.0 * 90;
	double s = AXLE_LENGTH * PI * angle_per_circle;
	double v = new_speed * T;
	double bs = s/v;
	double time_turn = calculateTime(1.0 * 90, 1.0 * speed);
	left_motor_set_speed(speed);
	right_motor_set_speed(-speed);
    chThdSleepMilliseconds(bs * 1000);

	left_motor_set_speed(0);
	right_motor_set_speed(0);
    chThdSleepMilliseconds(1000);

//	left_motor_set_speed(-speed);
//	right_motor_set_speed(speed);
//    chThdSleepMilliseconds(time_turn * 1000);
//	left_motor_set_speed(0);
//	right_motor_set_speed(0);
//    chThdSleepMilliseconds(1000);
    moveForward(speed);
}



void stop(void){
	left_motor_set_speed(0);
	right_motor_set_speed(0);
}

void parseMovement(char* movementSet, int len, int speed) {
	for(int i = 0; i < len; i++) {
		char movement = movementSet[i];
		switch(movement){
		case 'W':
			moveForward(speed);
			break;
		case 'S':
			moveBackward(speed);
			break;
		case 'A':
			turnLeft(speed);
			break;
		case 'D':
			turnRight(speed);
			break;
		}
	}
}
