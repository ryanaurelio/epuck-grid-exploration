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

#include "arm_math.h"
#include "arm_const_structs.h"

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

static BSEMAPHORE_DECL(rec_buffer_filled, true);
static BSEMAPHORE_DECL(fft_computed, true);

//static THD_WORKING_AREA(selector_thd_wa, 2048);
static THD_WORKING_AREA(selector_thd_wa, 1024);
static THD_WORKING_AREA(record_thd_wa, 2048);
static THD_WORKING_AREA(comm_thd_wa, 216);
static THD_WORKING_AREA(working_thd_wa, 2048);
//static THD_WORKING_AREA(complete_thd_wa, 65636);
static THD_WORKING_AREA(complete_thd_wa, 512);
//static THD_WORKING_AREA(yeye_thd_wa, 128);

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
	// Map the frequency detected to the RGB LEDs.
	return (uint16_t)(curr_freq);
}


void bc(int number) {
	chThdSleepMilliseconds(50);
//	switch(number) {
//	case 0:
//		dac_play(1275);
//		break;
//	case 1:
//		dac_play(1500);
//		break;
//	case 2:
//		dac_play(1700);
//		break;
//
////	case 3:
//	default:
//		dac_play((number * 200) + 1325);
//		break;
//
//	}
	dac_play((number * 300) + 1350);
	chThdSleepMilliseconds(200);
	dac_stop();
	chThdSleepMilliseconds(375);
}

void broadcast_num(int number, int can_be_negative) {
	int num = number;
	if (can_be_negative) {
		if (num >= 0) {
			bc(0);
			//add here
			bc(0);
		} else {
			//add here
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
//			num_list[index] = (num % 3);
//			num = num / 3;
			num_list[index] = (num % 2);
			num = num / 2;
			index++;
		}

		while(index != 0) {
			bc(num_list[--index]);
		}
	}
}

void broadcast(int x, int y, int num) {
	/*
	 * num: 0 for wall, 1 for unexplored, 2 for exploring.
	 */
	broadcast_num(get_my_id(), 0);
//	bc(3);
	bc(2);
	broadcast_num(x, 1);
//	bc(3);
	bc(2);
	broadcast_num(y, 1);
//	bc(3);
	bc(2);
	broadcast_num(num, 0);
//	bc(3);
	bc(2);
}

void broadcast_done(void) {
	broadcast_num(get_my_id(), 0);
//	bc(3);
	bc(2);
//	bc(2);
	bc(1);
	bc(1);
//	bc(3);
	bc(2);
}

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


void check_wall(void) {

	int pos_x = get_robot_with_index(get_my_id() - 1) -> coordinate -> x;
	int pos_y = get_robot_with_index(get_my_id() - 1) -> coordinate -> y;
	direction direction = get_robot_with_index(get_my_id() - 1) -> direction;

	//assumption : tof sensor has a noise for +- 0.5 cm. So, we check if there is obj within at least 6.5 cm in front. (diameter of epuck is 7cm).
	//here we check how long are the distance until we see the object. in the website, it is stated that tof sensor can see up to 2 m.
	//that means it can detect ~25 grid in front.
	//turn left 4 times
	int move = 4;
	while (move) {
		turnLeft(550);
		switch(direction) {
		case UP:
			direction = LEFT;
			break;
		case DOWN:
			direction = RIGHT;
			break;
		case LEFT:
			direction = DOWN;
			break;
		case RIGHT:
			direction = UP;
			break;
		}

		int sum = 0;
		for(int i = 0; i < 25; i++) {
			uint16_t dist = VL53L0X_get_dist_mm();
			sum += dist;
		}

		switch(direction) {
			case UP:
				check_map(&map, pos_x, pos_y + 1);
				if (get_symbol_dmap(map, pos_x, pos_y + 1) == 'u') {
					if(checkBroadcast(2 + (0.2 * get_my_id()))) {
						if (sum / 25 < (80 + 35)) {
							set_symbol_dmap(map, pos_x, pos_y + 1, 'x');
								broadcast(pos_x, pos_y + 1, 0);
						} else {
							//TODO broadcast
							set_symbol_dmap(map, pos_x, pos_y + 1, '.');
							broadcast(pos_x, pos_y + 1, 1);
						}
						canBroadcast = 0;
					}
				}
				break;
			case DOWN:
				check_map(&map, pos_x, pos_y - 1);
				if (get_symbol_dmap(map, pos_x, pos_y - 1) == 'u') {
					if(checkBroadcast(2 + (0.2 * get_my_id()))) {
						if (sum / 25 < (80 + 35)) {
							set_symbol_dmap(map, pos_x, pos_y - 1, 'x');
							broadcast(pos_x, pos_y - 1, 0);
						} else {
							//TODO broadcast
							set_symbol_dmap(map, pos_x, pos_y - 1, '.');
							broadcast(pos_x, pos_y - 1, 1);
						}
						canBroadcast = 0;
					}
				}
				break;
			case LEFT:
				check_map(&map, pos_x - 1, pos_y);
				if (get_symbol_dmap(map, pos_x - 1, pos_y) == 'u') {
					if(checkBroadcast(2 + (0.2 * get_my_id()))) {
						if (sum / 25 < (80 + 35)) {
							set_symbol_dmap(map, pos_x - 1, pos_y, 'x');
							broadcast(pos_x - 1, pos_y, 0);
						} else {
							//TODO broadcast
							set_symbol_dmap(map, pos_x - 1, pos_y, '.');
							broadcast(pos_x - 1, pos_y, 1);
						}
					canBroadcast = 0;
					}
				}
				break;
			case RIGHT:
				check_map(&map, pos_x + 1, pos_y);
				if (get_symbol_dmap(map, pos_x + 1, pos_y) == 'u') {
					if(checkBroadcast(2 + (0.2 * get_my_id()))) {
						if (sum / 25 < (80 + 35)) {
							set_symbol_dmap(map, pos_x + 1, pos_y, 'x');
							broadcast(pos_x + 1, pos_y, 0);
						} else {
							//TODO broadcast
							set_symbol_dmap(map, pos_x + 1, pos_y, '.');
							broadcast(pos_x + 1, pos_y, 1);
						}
						canBroadcast = 0;
					}
				}
				break;
		}
		move--;
	}
}

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


					 //adding this
					chThdSleepMilliseconds(500);
					check_wall();
					chThdSleepMilliseconds(300);

					set_rgb_led(LED2, 0, 0 ,1);
					Coordinate * nearestCoordinate;
					go_work();

					nearestCoordinate = get_nearest_coordinate_dmap(*map, get_robot_with_index(get_my_id() - 1) -> coordinate -> x ,
							get_robot_with_index(get_my_id() - 1) -> coordinate -> y);


					if(checkBroadcast(5 + (0.2 * get_my_id()))) {
						//2 means robot move
						broadcast((int)nearestCoordinate -> x, (int)nearestCoordinate -> y, 2);

						canBroadcast = 0;
						chThdSleepMilliseconds(100);


						move_robot_in_dmap(*map, get_my_id(), *get_robot_with_index(get_my_id() - 1) -> coordinate, *nearestCoordinate);



//						if(get_symbol_dmap(map, 0, -2) == '1') {
//												set_body_led(1);
//											}
//							set_front_led(1);
//							chThdSleepMilliseconds(1000);
//							set_front_led(0);



						get_robot_with_index(get_my_id() - 1) -> coordinate->x = nearestCoordinate -> x;
						get_robot_with_index(get_my_id() - 1) -> coordinate->y = nearestCoordinate -> y;
					}

					while(1) {

						if(checkBroadcast(3 + (0.2 * get_my_id()))) {
							set_rgb_led(LED4, 0, 1, 0);
							//this is broadcast for free.
							//adding this
							broadcast_done();
							robot_moved_in_dmap(*map, get_my_id(), *get_robot_with_index(get_my_id() - 1) -> coordinate);
							push_to_free_robots_list(get_robot_with_index(get_my_id() - 1));
							canBroadcast = 0;
							set_rgb_led(LED4, 0, 0, 0);
							break;
						}
					}
				} else {
					is_complete = 1;
//					break;
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

static THD_FUNCTION(comm_thd, arg) {
    (void) arg;
    chRegSetThreadName(__FUNCTION__);

    int time_first = ST2S(chVTGetSystemTimeX());
    int time_now = ST2S(chVTGetSystemTimeX());

	uint16_t rgb_from_freq = 0;

	//trying to connect
    while ((time_now - time_first) < 1) {
		dac_play(2100);
		time_now = ST2S(chVTGetSystemTimeX());
    }
    dac_stop();
	chThdSleepMilliseconds(1000);

    time_first = ST2S(chVTGetSystemTimeX());
    time_now = ST2S(chVTGetSystemTimeX());

    //waiting for the reaction of earlier robots
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

			set_led(LED1, 0);

		} else {
			time_now = ST2S(chVTGetSystemTimeX());
		}
	}

	Coordinate * my_coordinate;
    my_coordinate = (Coordinate *) malloc(sizeof(Coordinate));
    my_coordinate->x = id - 1;
    my_coordinate->y = 0;
    new_robot(id, my_coordinate);
	check_map(&map, my_coordinate -> x, my_coordinate -> y);
    setRobot(map, id, my_coordinate -> x, my_coordinate -> y);


	set_my_id(id);
	id++;

	set_rgb_led(LED2, 0, 0, 0);

	time_first = ST2S(chVTGetSystemTimeX());
	time_now = ST2S(chVTGetSystemTimeX());
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
			dac_stop();
			chThdSleepMilliseconds(600);
		} else {
			time_now = ST2S(chVTGetSystemTimeX());
		}
    }
    set_rgb_led(LED4, 0, 0, 0);

    //idk but the first move is always faster done that its supposed to? we need to try more.
	left_motor_set_speed(-550);
	right_motor_set_speed(-550);
	chThdSleepMilliseconds(10);
	stop();
	connectPhase = 0;
}

static void mic_callback(int16_t *data, uint16_t num_samples) {
	rec_data = data;
	rec_num_samples = num_samples;
	chBSemSignal(&rec_buffer_filled);
	return;
}

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
//			if(!is_dmap_complete(*map)) {

				rgb_from_freq = listen();

				set_led(LED5, 0);
				set_led(LED1, 0);
				set_led(LED7, 0);
				set_led(LED3, 0);

//				if( rgb_from_freq > 1200 && rgb_from_freq < 1400) {
//					push_sound_list(&sounds, 0);
//					set_led(LED1, 1);
//					chThdSleepMilliseconds(425);
//				}
//
//				else if(rgb_from_freq >= 1400 && rgb_from_freq < 1600) {
//					push_sound_list(&sounds, 1);
//					set_led(LED3, 1);
//					chThdSleepMilliseconds(425);
//				}
//
//				else if(rgb_from_freq >= 1600 && rgb_from_freq < 1800) {
//					push_sound_list(&sounds, 2);
//					set_led(LED5, 1);
//					chThdSleepMilliseconds(425);
//				}

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

//				else if(rgb_from_freq >= 1600 && rgb_from_freq < 1800) {
//					push_sound_list(&sounds, 2);
//					set_led(LED5, 1);
//					chThdSleepMilliseconds(425);
//				}

				else if(rgb_from_freq >= 1800 && rgb_from_freq < 2100) {
					push_sound_list(&sounds, 10);
					set_led(LED7, 1);
					chThdSleepMilliseconds(425);
				}
				//this is for the finish sound (> 1800)
//				else if(rgb_from_freq >= 1800 && rgb_from_freq < 2200) {
//					push_sound_list(&sounds, 10);
//					set_led(LED7, 1);
//					chThdSleepMilliseconds(425);
//				}

//				else {
//					push_sound_list(&sounds, 10);
//					set_led(LED7, 1);
//					chThdSleepMilliseconds(425);
//				}


				if(sounds -> sound == 10) {
					int is_done = 0;
					int is_complete = 0;
					pop_sound_list(&sounds);

					int sum = 0;
					int pow = 0;
					while(!is_sound_list_empty(sounds)) {

						set_body_led(1);
						chThdSleepMilliseconds(20);
						set_body_led(0);

//						int num = pop_sound_list(&sounds);
//						if (sounds -> next == NULL && (index == 1 || index == 2)) {
//							int num1 = pop_sound_list(&sounds);
//							if (num1 == 0 && num == 1) {
//								sum = -sum;
//							}
//							if(num1 == 1 && num == 1) {
//								index = 0;
//								is_done = 1;
//								robot_moved_in_dmap(*map, saved_coordinate[0], *get_robot_with_index(saved_coordinate[0] - 1) -> coordinate);
//								push_to_free_robots_list(get_robot_with_index(saved_coordinate[0] - 1));
//
//								if(is_dmap_complete(*map)) {
//									is_complete = 1;
//								}
//							}
//						} else {
//							for(int i = 0; i < pow; i++) {
//								num *= 2;
//							}
//							sum += num;
//							pow++;
//						}


//						if (sounds -> next == NULL && (index == 1 || index == 2)) {
						if(index == 1 || index == 2) {
							if (sounds -> next -> next == NULL) {

	//							int num = pop_sound_list(&sounds);
								int num2 = pop_sound_list(&sounds);
								int num1 = pop_sound_list(&sounds);

								if (num1 == 0 && num2 == 1) {
									sum = -sum;
								}
								if(num1 == 1 && num2 == 1) {
									index = 0;
									is_done = 1;

									robot_moved_in_dmap(*map, saved_coordinate[0], *get_robot_with_index(saved_coordinate[0] - 1) -> coordinate);

									push_to_free_robots_list(get_robot_with_index(saved_coordinate[0] - 1));

									if(is_dmap_complete(*map)) {
										is_complete = 1;
									}
								}
							} else {
								int num = pop_sound_list(&sounds);
								for(int i = 0; i < pow; i++) {
	//								num *= 3;
									num *= 2;
								}
								sum += num;
								pow++;
							}


//							if (num == 1) {
//								sum = -sum;
//							}
//							if (num == 2) {
//								index = 0;
//
//								robot_moved_in_dmap(*map, saved_coordinate[0], *get_robot_with_index(saved_coordinate[0] - 1) -> coordinate);
//
//								push_to_free_robots_list(get_robot_with_index(saved_coordinate[0] - 1));
//
//								if(is_dmap_complete(*map)) {
//									is_complete = 1;
//								}
//							}
						} else {
							int num = pop_sound_list(&sounds);
							for(int i = 0; i < pow; i++) {
//								num *= 3;
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


					if(index == 1 || index == 2) {
						set_front_led(1);
					} else {
						set_front_led(0);
					}

					if(is_complete)
						break;
				}


//				}

//for fulfilling the saved_coordinate.
				if (index == 4) {
					index = 0;
					int robot_id = saved_coordinate[0];
					int pos_x = saved_coordinate[1];
					int pos_y = saved_coordinate[2];
					int indicator = saved_coordinate[3];
					check_map(&map, pos_x, pos_y);
					go_work();

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
// 						go_work();
						Coordinate * new_coordinate = (Coordinate*) malloc(sizeof(Coordinate));
						new_coordinate -> x = pos_x;
						new_coordinate -> y = pos_y;

						move_robot_in_dmap(*map, robot_id, *get_robot_with_index(robot_id - 1) -> coordinate, *new_coordinate);

						get_robot_with_index(robot_id - 1) -> coordinate -> x = pos_x;
						get_robot_with_index(robot_id - 1) -> coordinate -> y = pos_y;
					}

				}

//			}

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
	set_body_led(0);
	set_front_led(0);
    mic_start(mic_callback);
//    usb_start();
    dac_start();
    spi_comm_start();
    VL53L0X_start();
//    sdio_start();
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
