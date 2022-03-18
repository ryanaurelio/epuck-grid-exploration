#include <string.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "motors.h"
#include "sensors/proximity.h"
#include "leds.h"

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

void stop(void){
	left_motor_set_speed(0);
	right_motor_set_speed(0);
}
void moveForward(int speed){
	left_motor_set_speed(speed);
	right_motor_set_speed(speed);
//    chThdSleepMilliseconds(1000);
    chThdSleepMilliseconds(1500);
    stop();
}

void moveBackward(int speed){
	left_motor_set_speed(-speed);
	right_motor_set_speed(-speed);
//    chThdSleepMilliseconds(1000);

    chThdSleepMilliseconds(1500);
    stop();
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
	left_motor_set_speed(speed);
	right_motor_set_speed(-speed);
//    chThdSleepMilliseconds(bs * 1000);
    chThdSleepMilliseconds(581.77);

	left_motor_set_speed(0);
	right_motor_set_speed(0);
    chThdSleepMilliseconds(1000);

}

void turnLeft(int speed) {
	double new_speed = MAX_VELOCITY * speed / 1000;
	double T = 1.0 / (AXLE_LENGTH * PI / find_speed(speed));
	double angle_per_circle = 1.0 / 360.0 * 90;
	double s = AXLE_LENGTH * PI * angle_per_circle;
	double v = new_speed * T;
	double bs = s/v;
	double time_turn = calculateTime(1.0 * 90, 1.0 * speed);
//	left_motor_set_speed(speed);
//	right_motor_set_speed(-speed);
//    chThdSleepMilliseconds(bs * 1000);
//
//	left_motor_set_speed(0);
//	right_motor_set_speed(0);
//    chThdSleepMilliseconds(1000);

	left_motor_set_speed(-speed);
	right_motor_set_speed(speed);
//    chThdSleepMilliseconds(time_turn * 1000);
	chThdSleepMilliseconds(581.77);
	left_motor_set_speed(0);
	right_motor_set_speed(0);
    chThdSleepMilliseconds(1000);

}





void parseMovement(char* movementSet, int len, int speed) {
//	if ((int)strlen(movementSet) > 1) {
//		set_body_led(1);
//		chThdSleepMilliseconds(1000);
//	}

	for(int i = 0; i < (int)strlen(movementSet); i++) {
		char movement = movementSet[i];
		switch(movement){
		case 'W':
			set_led(LED1, 1);
			moveForward(speed);
			chThdSleepMilliseconds(1000);
			set_led(LED1, 0);
			break;
		case 'S':
			set_led(LED5, 1);
			moveBackward(speed);
			chThdSleepMilliseconds(1000);
			set_led(LED5, 0);
			break;
		case 'A':
			set_led(LED7, 1);
			turnLeft(speed);
			moveForward(speed);
			turnRight(speed);
			chThdSleepMilliseconds(1000);
			set_led(LED7, 0);
			break;
		case 'D':
			set_led(LED3, 1);
			turnRight(speed);
			moveForward(speed);
			turnLeft(speed);
			chThdSleepMilliseconds(1000);
			set_led(LED3, 0);
			break;
		}
	}



}
