#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include "audio/audio_thread.h"
#include "record.h"
#include "robot.h"
#include "dmap_pathfinder.h"
#include "dynamic_map.h"
#include "leds.h"

static THD_WORKING_AREA(working_thd_wa, 2048);

static thread_t * working_thd_handle = NULL;

//main broadcast function
void bc(int number) {
	chThdSleepMilliseconds(50);
	dac_play((number * 300) + 1350);
	chThdSleepMilliseconds(200);
	dac_stop();
	chThdSleepMilliseconds(375);
}

//sub-function for broadcast
/*
 * because robot id and indicator can't be negative, to save time we have can_be_negative value
 * 00xxx : positive
 * 01xxx : negative
 * 11	 : done
 */
void broadcast_num(int number, int can_be_negative) {
	int num = number;
	if (can_be_negative) {
		if (num >= 0) {
			bc(0);
			bc(0);
		} else {
			bc(0);
			bc(1);
			num = -1 * num;
		}
	}
	if (num == 0) {
		 bc(0);
	} else {
		int num_list[(int)sqrt(num) + 1];
		int index = 0;
		while(num != 0) {
			num_list[index] = (num % 2);
			num = num / 2;
			index++;
		}
		while(index != 0) {
			bc(num_list[--index]);
		}
	}
}

//broadcast for the coordinate
/*
 * num is an indicator
 * num: 0 for wall, 1 for unexplored, 2 for exploring.
 * 2 is a separator
 */
void broadcast(int x, int y, int num) {
	broadcast_num(get_my_id(), 0);
	bc(2);
	broadcast_num(x, 1);
	bc(2);
	broadcast_num(y, 1);
	bc(2);
	broadcast_num(num, 0);
	bc(2);
}

//broadcast for the obstacle and unexplored area near the robot
/*
 * this checks the map near the robot and broadcast it, whether it is an obstacle or unexplored
 * 'x' is obstacle
 * '.' is unexplored
 */
void broadcast_wall(dmap * map, int x, int y, int tof_value) {
	check_map(&map, pos_x, pos_y);
	if (get_symbol_dmap(map, pos_x, pos_y) == 'u') {
		if(checkBroadcast(2 + (0.2 * get_my_id()))) {
			if (tof_value / 25 < (80 + 35)) {
				set_symbol_dmap(map, pos_x, pos_y, 'x');
					broadcast(pos_x, pos_y, 0);
			} else {
				set_symbol_dmap(map, pos_x, pos_y, '.');
				broadcast(pos_x, pos_y, 1);
			}
			canBroadcast = 0;
		}
	}
}

//wait for the situation to be silent, so that no robot can broadcast at the same time
int checkBroadcast(int number) {
	int rgb_from_freq;
	int time_first = ST2S(chVTGetSystemTimeX());
	int time_now = ST2S(chVTGetSystemTimeX());
	while((time_now - time_first) < number) {
		time_now = ST2S(chVTGetSystemTimeX());
		rgb_from_freq = listen();
		if(rgb_from_freq > 1200) {
			time_first = ST2S(chVTGetSystemTimeX());
		}
	}
	canBroadcast = 1;
	return 1;
}

/*
 * the robot rotates 4 times to check the nearby area and broadcast them.
 */
void check_wall(void) {

	int pos_x = get_robot_with_index(get_my_id() - 1) -> coordinate -> x;
	int pos_y = get_robot_with_index(get_my_id() - 1) -> coordinate -> y;
	direction direction = get_robot_with_index(get_my_id() - 1) -> direction;

	//turn left 4 times
	int move = 4;
	while (move) {
		turnLeft(550);
		int sum = 0;
		for(int i = 0; i < 25; i++) {
			uint16_t dist = VL53L0X_get_dist_mm();
			sum += dist;
		}

		switch(direction) {
		case UP:
			direction = LEFT;
			broadcast_wall(map, pos_x, pos_y + 1, sum);
			break;
		case DOWN:
			direction = RIGHT;
			broadcast_wall(map, pos_x, pos_y - 1, sum);
			break;
		case LEFT:
			direction = DOWN;
			broadcast_wall(map, pos_x - 1, pos_y, sum);
			break;
		case RIGHT:
			direction = UP;
			broadcast_wall(map, pos_x + 1, pos_y, sum);
			break;
		}
		move--;
	}
}

/*
 * this function is called to broadcast the move before the robot itself moves.
 */
void broadcast_work(dmap * map) {
	Coordinate * nearestCoordinate;
	nearestCoordinate = get_nearest_coordinate_dmap(*map);

	if(checkBroadcast(5 + (0.2 * get_my_id()))) {
		//2 means robot move
		broadcast((int)nearestCoordinate -> x, (int)nearestCoordinate -> y, 2);
		canBroadcast = 0;
		chThdSleepMilliseconds(100);
		move_robot_in_dmap(*map, get_my_id(), *nearestCoordinate);
	}

	free(nearestCoordinate);
}

/*
 * this function is called to broadcast if the work is done
 */
void broadcast_done(dmap * map) {
	while(1) {
		if(checkBroadcast(3 + (0.2 * get_my_id()))) {
			set_rgb_led(LED4, 0, 1, 0);
			//this is broadcast for free.
			broadcast_num(get_my_id(), 0);
			bc(2);
			bc(1);
			bc(1);
			bc(2);
			robot_moved_in_dmap(*map, get_my_id());
			canBroadcast = 0;
			set_rgb_led(LED4, 0, 0, 0);
			break;
		}
	}
}

static THD_FUNCTION(working_thd, arg) {
    (void) arg;
    chRegSetThreadName(__FUNCTION__);

    int is_complete = 0;
    //todo getmap
    while(1) {
    	if(is_complete) {
    		break;
    	}
			 if(can_work()) {

				 if(!is_dmap_complete(*map)) {

					chThdSleepMilliseconds(500);
					check_wall();
					chThdSleepMilliseconds(300);

					set_rgb_led(LED2, 0, 0 ,1);
					go_work();
					broadcast_work();
					broadcast_done();
				} else {
					is_complete = 1;
				}
			}

			//very important!
		else {
			set_rgb_led(LED6, 0, 0 , 1);
		}
    }

    set_rgb_led(LED6,0, 0, 0);
	set_front_led(1);
	chThdSleepMilliseconds(5000);
	set_front_led(0);
	chThdSleepMilliseconds(100);
}

void work_start(void) {
	working_thd_handle = chThdCreateStatic(working_thd_wa, sizeof(working_thd_wa), NORMALPRIO, working_thd, NULL);
}

void work_stop(void) {
    chThdTerminate(working_thd_handle);
    chThdWait(working_thd_handle);
    working_thd_handle = NULL;
}
