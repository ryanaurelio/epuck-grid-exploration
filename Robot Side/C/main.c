#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "memory_protection.h"
#include <main.h>
#include "audio/audio_thread.h"
#include "audio/microphone.h"
#include "audio/play_sound_file.h"
#include "sensors/proximity.h"
#include "sensors/VL53L0X/VL53L0X.h"
#include "button.h"
#include "leds.h"
#include "sdio.h"
#include "selector.h"
#include "spi_comm.h"
#include "usbcfg.h"

#include "audio/play_melody.h"
#include "sensors/proximity.h"
#include "motors.h"

#include "types.h"
#include "move.h"
#include "robot.h"
#include "dynamic_map.h"
#include "dmap_pathfinder.h"
#include "connect.h"

#include "arm_math.h"
#include "arm_const_structs.h"

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

static BSEMAPHORE_DECL(rec_buffer_filled, true);
static BSEMAPHORE_DECL(fft_computed, true);

static THD_WORKING_AREA(comm_thd_wa, 216);
static THD_WORKING_AREA(selector_thd_wa, 1024);
static THD_WORKING_AREA(record_thd_wa, 2048);
static THD_WORKING_AREA(working_thd_wa, 2048);

// Microphones data variables.
static int16_t *rec_data;
static uint16_t rec_num_samples;


// FFT variables and definitions.
#define FFT_ARRAY_LEN 2048 // The array length includes real and complex parts. The resolution of the FFT will be: 16000 (=> sampling frequency) / 1024 (=> FFT_ARRAY_LEN/2) = 15.625 Hz.
uint16_t mic0_data_complex_index = 0;
float32_t mic0_data_complex[FFT_ARRAY_LEN];
static float32_t fft_output[FFT_ARRAY_LEN/2];
uint32_t fftSize = FFT_ARRAY_LEN/2;
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
uint32_t fft_index = 0;
float32_t fft_max_value;

int connectPhase = 1;
int canBroadcast = 0;
int id = 1;
dmap * map;


int listen(void) {
	// Detect frequencies with FFT and playback tones based on the distance from ToF.
	// The buffer for the FFT will be filled every 64 ms:
	// - we get 160 samples (for each micro) every 10 ms
	// - the buffer is 1024 long => 1024/160 * 10 ms = 64 ms
	float32_t curr_freq = 0.0;
	chBSemWait(&fft_computed);
	if(fft_index < FFT_ARRAY_LEN/4) { // Can only detect up to 8 KHz signals when sampling @ 16 KHz.
		curr_freq = (float32_t)fft_index*16000.0/(float32_t)fftSize; // 16000 / 1024 = 15.625 Hz for each BIN.
	}
	return (uint16_t)(curr_freq);
}

//main broadcast function
void bc(int number) {
	//broadcast the number between 0, 1 or separator(2)
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
			//for positive x and y coordinate
			bc(0);
			bc(0);
		} else {
			//for negative x and y coordinate
			bc(0);
			bc(1);
			num = -1 * num;
		}
	}
	//if coordinate = 0
	if (num == 0) {
		 bc(0);
	} else {
		//if coordinate > 0
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

/*
 * this function is called to broadcast if the work is done
 */
void broadcast_done(void) {
	broadcast_num(get_my_id(), 0);
	bc(2);
	bc(1);
	bc(1);
	bc(2);
}

//wait for the situation to be silent, so that no robot can broadcast at the same time
int checkBroadcast(int number) {
	int rgb_from_freq;
	//take timestamp
	int time_first = ST2S(chVTGetSystemTimeX());
	int time_now = ST2S(chVTGetSystemTimeX());
	while((time_now - time_first) < number) {
		time_now = ST2S(chVTGetSystemTimeX());
		rgb_from_freq = listen();
		if(rgb_from_freq > 1200) {
			//if hears a sound > 1200 Hz, reset the timestamp
			time_first = ST2S(chVTGetSystemTimeX());
		}
	}
	canBroadcast = 1;
	return 1;
}

//broadcast for the obstacle and unexplored area near the robot
/*
 * this checks the map near the robot and broadcast it, whether it is an obstacle or unexplored
 * 'x' is obstacle
 * '.' is unexplored
 */
void broadcast_wall(dmap * map, int x, int y, int tof_value) {
	check_map(&map, pos_x, pos_y);
	//if the place is still unknown, broadcast it, otherwise just skip because it's already broadcasted previously
	if (get_symbol_dmap(map, pos_x, pos_y) == 'u') {
		if(checkBroadcast(2 + (0.2 * get_my_id()))) {
			if (tof_value / 25 < (80 + 35)) {
				set_symbol_dmap(map, pos_x, pos_y, 'x');
				//0 is wall
				broadcast(pos_x, pos_y, 0);
			} else {
				set_symbol_dmap(map, pos_x, pos_y, '.');
				//1 is unexplored
				broadcast(pos_x, pos_y, 1);
			}
			canBroadcast = 0;
		}
	}
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
		//take a tof value 25 times. This is because the tof value is not very accurate. We hope that with averaging it, we can get a ~equal value
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
 * This is the thread for work.
 * check:
 * - if the map is completed
 * - if it is not the connect phase
 * - if it is the robot's turn to work
 * how it works:
 * 1. the robot rotates 360 degrees to check any wall / unexplored area nearby and then broadcast them
 * 2. the robot then find a nearest unexplored area and broadcast that the robot will go there
 * 3. the robot moves
 * 4. after the robot arrives, it broadcasts that it's done
 */
static THD_FUNCTION(working_thd, arg) {
    (void) arg;
    chRegSetThreadName(__FUNCTION__);

    int is_complete = 0;
    while(1) {
    	if(is_complete) {
    		break;
    	}
		if(!connectPhase) {

			 if(can_work()) {

				 if(!is_dmap_complete(*map)) {

					 //1.
					chThdSleepMilliseconds(500);
					check_wall();
					chThdSleepMilliseconds(300);
					set_rgb_led(LED2, 0, 0 ,1);
					go_work();
					Coordinate * nearestCoordinate;
					nearestCoordinate = get_nearest_coordinate_dmap(*map);

					//2.
					if(checkBroadcast(5 + (0.2 * get_my_id()))) {
						//2 means robot move
						broadcast((int)nearestCoordinate -> x, (int)nearestCoordinate -> y, 2);
						canBroadcast = 0;
						chThdSleepMilliseconds(100);
						//3.
						move_robot_in_dmap(*map, get_my_id(), *nearestCoordinate);
					}
					//4.
					while(1) {

						if(checkBroadcast(3 + (0.2 * get_my_id()))) {
							set_rgb_led(LED4, 0, 1, 0);
							//this is broadcast for free.
							broadcast_done();
							robot_moved_in_dmap(*map, get_my_id());
							push_to_free_robots_list(get_robot_with_index(get_my_id() - 1));
							canBroadcast = 0;
							set_rgb_led(LED4, 0, 0, 0);
							break;
						}
					}
				} else {
					is_complete = 1;
				}
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
	connectPhase = 0;
}

//this function is taken from example dsp on epuck website
static void mic_callback(int16_t *data, uint16_t num_samples) {
	rec_data = data;
	rec_num_samples = num_samples;
	chBSemSignal(&rec_buffer_filled);
	return;
}

//this function is taken from example dsp on epuck website
static THD_FUNCTION(record_thd, arg) {
	(void) arg;
	chRegSetThreadName(__FUNCTION__);
	uint16_t i = 0;
	uint16_t j = 0;

	while(1) {
		chBSemWait(&rec_buffer_filled);

		//set_led(LED3, 1);

		for(i=0; i<rec_num_samples; i+=4) { // Consider only one microphone for FFT.
			mic0_data_complex[mic0_data_complex_index++] = rec_data[i];
			mic0_data_complex[mic0_data_complex_index++] = 0; // Set complex part to zero.

			if(mic0_data_complex_index == FFT_ARRAY_LEN) {
				mic0_data_complex_index = 0;

				//set_led(LED5, 1);

				// Process the data through the CFFT/CIFFT module.
				arm_cfft_f32(&arm_cfft_sR_f32_len1024, mic0_data_complex, ifftFlag, doBitReverse);

				// Process the data through the Complex Magnitude Module for calculating the magnitude at each bin.
				arm_cmplx_mag_f32(mic0_data_complex, fft_output, fftSize);

				// Set the magnitude to zero in the "fft_output" array starting from index 148 to not consider frequencies >= 16000/1024*148 = 2312.5 Hz.
				// This is done to avoid incorrectly detecting low frequencies emitted by the robot itself (e.g. when the robot emits about 800 Hz then
				// the FFT often detects around 3 KHz and the RGB colors continuously pass from blue to red and vice versa).
				// Basically this is only a workaround to let the demo behave correctly.
				for(j=148; j<(FFT_ARRAY_LEN/2); j++) {
					fft_output[j] = 0.0;
				}

				// Calculates maxValue and returns corresponding BIN value.
				arm_max_f32(fft_output, fftSize, &fft_max_value, &fft_index);

				chBSemSignal(&fft_computed);

				//set_led(LED5, 0);
			}
		}

		//set_led(LED3, 0);

	}
}

/*
 * this is the listening thread
 * checks:
 * - if the map is complete
 * - if it's not the connectphase
 * - if the robot is not broadcasting. This is because the robot hears its own sound
 * how it works:
 * 1. the robot tried to listen for a sound between 1200 - 2100 Hz
 * 2. the robot fills the sound in the sounds list
 * 3. if the robot hears a separator, calculate the sound in sounds list and fill it on the saved_coordinate
 * 4. after filling the saved_coordinate until 4th index, do the function from the broadcasted message from others (change symbol to a wall/unexplored/robot exploring)
 * for the second index , the robot will hear ooxxx (oo is indicator if its positive/negative/done, x is coordinate, see on broadcast function for detailed info)
 * 5. if on the second index, the robot hears 11 (done), the robot pops the working robot list and marks the other robot as done doing its job
 */
static THD_FUNCTION(selector_thd, arg) {
    (void) arg;
    chRegSetThreadName(__FUNCTION__);

	uint16_t rgb_from_freq = 0;
	sound_node * sounds = (sound_node*) malloc(sizeof(sound_node));
	new_sound_list(sounds);
	int saved_coordinate[4];
	int index = 0;
	while(1) {

		if(!canBroadcast && !connectPhase) {
			//1.
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

			//2.
			if(sounds -> sound == 10) {
				int is_done = 0;
				int is_complete = 0;
				pop_sound_list(&sounds);

				int sum = 0;
				int pow = 0;
				//3.
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
						//5.
						if(num1 == 1 && num == 1) {
							index = 0;
							is_done = 1;
							robot_moved_in_dmap(*map, saved_coordinate[0], *get_robot_with_index(saved_coordinate[0] - 1) -> coordinate);
							push_to_free_robots_list(get_robot_with_index(saved_coordinate[0] - 1));

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
				saved_coordinate[index] = sum;
				if(!is_done) {
					index++;
				} else {
					is_done = 0;
				}
				if(is_complete)
					break;
			}

			//4.
			//after the saved_coordinate is all saved (robot_id, x, y, indicator), time to set the symbol on dmap
			if (index == 4) {
				index = 0;
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
					go_work();
					Coordinate * new_coordinate = (Coordinate*) malloc(sizeof(Coordinate));
					new_coordinate -> x = pos_x;
					new_coordinate -> y = pos_y;

					move_robot_in_dmap(*map, robot_id, *new_coordinate);

					get_robot_with_index(robot_id - 1) -> coordinate -> x = pos_x;
					get_robot_with_index(robot_id - 1) -> coordinate -> y = pos_y;
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

int main(void) {

    halInit();
    chSysInit();
    mpu_init();

    // Inits the Inter Process Communication bus.
    messagebus_init(&bus, &bus_lock, &bus_condvar);

    // Init the peripherals.
	clear_leds();
    mic_start(mic_callback);
	set_body_led(0);
	set_front_led(0);
    dac_start();
    spi_comm_start();
    VL53L0X_start();
    playSoundFileStart();
    motors_init();

    map = (dmap*) malloc(sizeof(dmap));
    new_dmap(map);
    init_robots();

    chThdCreateStatic(record_thd_wa, sizeof(record_thd_wa), NORMALPRIO, record_thd, NULL);
	chThdCreateStatic(comm_thd_wa, sizeof(comm_thd_wa), NORMALPRIO, comm_thd, NULL);
	chThdCreateStatic(selector_thd_wa, sizeof(selector_thd_wa), NORMALPRIO, selector_thd, NULL);
	chThdCreateStatic(working_thd_wa, sizeof(working_thd_wa), NORMALPRIO, working_thd, NULL);

    /* Infinite loop. */
    while (1) {
    	chThdSleepMilliseconds(1000);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void) {
    chSysHalt("Stack smashing detected");
}
