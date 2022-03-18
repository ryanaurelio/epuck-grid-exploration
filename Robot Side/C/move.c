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

void stop(void){
	left_motor_set_speed(0);
	right_motor_set_speed(0);
}
void moveForward(int speed){
	left_motor_set_speed(speed);
	right_motor_set_speed(speed);
    chThdSleepMilliseconds(1500);
    stop();
}

void moveBackward(int speed){
	left_motor_set_speed(-speed);
	right_motor_set_speed(-speed);
    chThdSleepMilliseconds(1500);
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

