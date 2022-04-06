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


static THD_WORKING_AREA(selector_thd_wa, 1024);

static thread_t * selector_thd_handler = NULL;

/*
 * this calculate the value of sounds after separator
 */
sound_node * calculate_sound(sound_node * sounds, int index, int robot_id) {
	sound_node * retval = (sound_node *) malloc(sizeof(sound_node));
	new_sound_list(retval);
	int is_done = 0;
	int is_complete = 0;
	pop_sound_list(&sounds);

	int sum = 0;
	int pow = 0;
	while(!is_sound_list_empty(sounds)) {

		set_body_led(1);
		chThdSleepMilliseconds(20);
		set_body_led(0);

		int num = pop_sound_list(&sounds);
		if (sounds -> next == NULL && (index == 1 || index == 2)) {
			int num1 = pop_sound_list(&sounds);
			if (num1 == 0 && num == 1) {
				sum = -sum;
			}
			if(num1 == 1 && num == 1) {
				index = 0;
				is_done = 1;
				robot_moved_in_dmap(*map, robot_id);

				if(is_dmap_complete(*map)) {
					is_complete = 1;
				}
			}
		} else {
			for(int i = 0; i < pow; i++) {
				num *= 2;
			}
			sum += num;
			pow++;
		}
	}

	if(is_complete)
		push_sound_list(&retval, 1);
	 else
		push_sound_list(&retval, 0);

	if(is_done)
		push_sound_list(&retval, 1);
	 else
		push_sound_list(&retval, 0);

	push_sound_list(&retval, sum);

	return retval;

}

/*
 * here the robot will try to listen to the sounds and updates the map according to the sound heard
 */
static THD_FUNCTION(selector_thd, arg) {
    (void) arg;
    chRegSetThreadName(__FUNCTION__);

	uint16_t rgb_from_freq = 0;
	sound_node * sounds = (sound_node*) malloc(sizeof(sound_node));
	new_sound_list(sounds);
	int saved_coordinate[4] = {0,0,0,0};
	int index = 0;
	dmap * map = get_map();
	while(1) {

		if(!canBroadcast) {

				rgb_from_freq = listen();

				set_led(LED5, 0);
				set_led(LED1, 0);
				set_led(LED7, 0);
				set_led(LED3, 0);

				if( rgb_from_freq > 1200 && rgb_from_freq < 1500) {
					push_sound_list(&sounds, 0);
					set_led(LED1, 1);
					chThdSleepMilliseconds(425);
				}

				else if(rgb_from_freq >= 1500 && rgb_from_freq < 1800) {
					push_sound_list(&sounds, 1);
					set_led(LED3, 1);
					chThdSleepMilliseconds(425);
				}

				else if(rgb_from_freq >= 1800 && rgb_from_freq < 2100) {
					push_sound_list(&sounds, 10);
					set_led(LED7, 1);
					chThdSleepMilliseconds(425);
				}


				if(sounds -> sound == 10) {
					sound_node * retval = calculate_sound(sounds, index, saved_coordinate[0]);
					saved_coordinate[index] = pop_sound_list(&retval);

					if(!pop_sound_list(&retval))
						index++;

					if(pop_sound_list(&retval))
						break;

					free(retval);
				}

				//for fulfilling the saved_coordinate.
				if (index == 4) {
					index = 0;
					go_work();
					int robot_id = saved_coordinate[0];
					int pos_x = saved_coordinate[1];
					int pos_y = saved_coordinate[2];
					int indicator = saved_coordinate[3];
					check_map(&map, pos_x, pos_y);

					/*
					 * num: 0 for wall, 1 for unexplored, 2 for exploring.
					 */
					if (indicator == 0) {
						set_symbol_dmap(map, pos_x, pos_y, 'x');

					}
					if (indicator == 1) {
						set_symbol_dmap(map, pos_x, pos_y, '.');
					}

					if (indicator == 2) {
						Coordinate * new_coordinate = (Coordinate*) malloc(sizeof(Coordinate));
						new_coordinate -> x = pos_x;
						new_coordinate -> y = pos_y;
						move_robot_in_dmap(*map, robot_id, *new_coordinate);
						free(new_coordinate);
					}

				}

		}
		//this shows it's not listening.
					else {
						set_rgb_led(LED8, 1, 0, 0);
					}
					set_rgb_led(LED8, 0 ,0 ,0);
	}

    set_rgb_led(LED6,0, 0, 0);
	set_body_led(1);
	chThdSleepMilliseconds(5000);
	set_body_led(0);
	chThdSleepMilliseconds(100);
}

void listen_start(void) {
	selector_thd_handler = chThdCreateStatic(selector_thd_wa, sizeof(selector_thd_wa), NORMALPRIO, selector_thd, NULL);
}

void listen_stop(void) {
    chThdTerminate(selector_thd_handler);
    chThdWait(selector_thd_handler);
    selector_thd_handler = NULL;
}

