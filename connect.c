#include <stdio.h>
#include <stdlib.h>

#include "ch.h"
#include "hal.h"
#include "types.h"
#include "record.h"
#include "motors.h"
#include "robot.h"
#include "dmap_pathfinder.h"
#include "dynamic_map.h"
#include "leds.h"
#include "move.h"
#include "audio/audio_thread.h"

static thread_t * connect_thd_handle = NULL;
int connect_finish = 0;
static THD_WORKING_AREA(connect_thd_wa, 256);

/*
 * in this thread the robots will try to connect with each other.
 * how it works:
 * 1. robot A will produce a sound
 * 2. the previous robots that has been started before will produce a sound as a reply. Robot A hears the sound and save the previous robots in the list of all robots
 * 3. robot A will wait for another next robot's sound.
 */
static THD_FUNCTION(connect_thd, arg) {
    (void) arg;
    chRegSetThreadName(__FUNCTION__);
    id = 1;
    uint16_t rgb_from_freq = 0;
    //TODO get dmap

	//trying to connect
    //1.
	dac_play(2100);
	chThdSleepMilliseconds(1000);
    dac_stop();
	chThdSleepMilliseconds(1000);

    int time_first = ST2S(chVTGetSystemTimeX());
    int time_now = ST2S(chVTGetSystemTimeX());

    //2.
    //waiting for the reaction of earlier robots
    //repeating the timer after getting an answer from another robot
	while((time_now - time_first) < 5) {
		rgb_from_freq = listen();
		set_rgb_led(LED2, 0, 0, 1);
		if (rgb_from_freq > 2000) {
			time_first = ST2S(chVTGetSystemTimeX());
			set_led(LED1, 1);
			chThdSleepMilliseconds(1000);
			Coordinate * new_coordinate = (Coordinate*) malloc(sizeof(Coordinate));

			new_coordinate -> x = id - 1;
			new_coordinate -> y = 0;
			new_robot(id, new_coordinate);

			check_map(&map, new_coordinate -> x, new_coordinate -> y);
			setRobot(map, id, new_coordinate -> x, new_coordinate -> y);
			id++;
			free(new_coordinate);
			set_led(LED1, 0);
		} else {
			time_now = ST2S(chVTGetSystemTimeX());
		}
	}

	//pushing this robot to the list of robots
	Coordinate * my_coordinate;
    my_coordinate = (Coordinate *) malloc(sizeof(Coordinate));
    my_coordinate->x = id - 1;
    my_coordinate->y = 0;
    new_robot(id, my_coordinate);
	check_map(&map, my_coordinate -> x, my_coordinate -> y);
    setRobot(map, id, my_coordinate -> x, my_coordinate -> y);
	set_my_id(id);
	id++;
	free(my_coordinate);
	set_rgb_led(LED2, 0, 0, 0);

	time_first = ST2S(chVTGetSystemTimeX());
	time_now = ST2S(chVTGetSystemTimeX());

	//3.
	//waiting for the next robots
    while((time_now - time_first) < 5) {

		set_rgb_led(LED4, 0, 0, 1);
		rgb_from_freq = listen();
		if(rgb_from_freq > 2000) {
			chThdSleepMilliseconds(2000);
			chThdSleepMilliseconds(500);
			dac_play(2100);
			time_first = ST2S(chVTGetSystemTimeX()) + 5;
			chThdSleepMilliseconds(400);
			Coordinate * new_coordinate = (Coordinate*) malloc(sizeof(Coordinate));
			new_coordinate -> x = id - 1;
			new_coordinate -> y = 0;
			check_map(&map, new_coordinate -> x, new_coordinate -> y);
			setRobot(map, id, new_coordinate -> x, new_coordinate -> y);
			new_robot(id, new_coordinate);
			id++;
			free(new_coordinate);
			dac_stop();
			chThdSleepMilliseconds(600);
		} else {
			time_now = ST2S(chVTGetSystemTimeX());
		}
    }
    set_rgb_led(LED4, 0, 0, 0);

    //starting the engine. It's rather important because if we don't use it the first move is not moving correctly.
	left_motor_set_speed(-550);
	right_motor_set_speed(-550);
	chThdSleepMilliseconds(10);
	stop();
	connect_finish = 1;
}

void connect_start(void) {
    connect_thd_handle = chThdCreateStatic(connect_thd_wa, sizeof(connect_thd_wa), NORMALPRIO, connect_thd, NULL);
}

void connect_stop(void) {
    chThdTerminate(connect_thd_handle);
    chThdWait(connect_thd_handle);
    connect_thd_handle = NULL;
}

int connect_finished(void) {
	if(connect_finish)
		connect_stop();
	return connect_finish;
}

