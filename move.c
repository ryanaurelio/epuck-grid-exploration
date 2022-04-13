//#include "move.h"

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
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


//static THD_WORKING_AREA(proximity_thd_wa, 512);


void stop(void){
	left_motor_set_speed(0);
	right_motor_set_speed(0);
}

void moveBackward(int speed, int loop){
//	for (int i = 0 ; i <= loop ; i ++) {
//		left_motor_set_speed(-speed);
//		right_motor_set_speed(-speed);
//	    chThdSleepMilliseconds(100);
//	    stop();
//	    chThdSleepMilliseconds(500);
//	}


	left_motor_set_speed(-speed);
	right_motor_set_speed(-speed);
	chThdSleepMilliseconds(loop * 100);
	stop();
}

void moveForward(int speed){
//	for (int i = 0 ; i < 15 ; i ++) {
//		calibrate_ir();
//		if (get_prox(0) > 500) {
//			stop();
//			moveBackward(speed, i);
//			break;
//		}
//		left_motor_set_speed(speed);
//		right_motor_set_speed(speed);
//	    chThdSleepMilliseconds(100);
//	    stop();
//	}
//
//
	left_motor_set_speed(speed);
	right_motor_set_speed(speed);
//	left_motor_set_speed(1000);
//	right_motor_set_speed(1000);
	chThdSleepMilliseconds(1500);
//	chThdSleepMilliseconds(825);
	stop();
}



void turnRight(int speed) {
	left_motor_set_speed(speed);
	right_motor_set_speed(-speed);
    chThdSleepMilliseconds(581.77);
    stop();

}

void turnLeft(int speed) {
	left_motor_set_speed(-speed);
	right_motor_set_speed(speed);
	chThdSleepMilliseconds(581.77);
	stop();
}

