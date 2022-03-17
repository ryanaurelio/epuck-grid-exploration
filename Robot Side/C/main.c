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
#include "map.h"
#include "robot.h"
#include "pathfinder.h"

#include "arm_math.h"
#include "arm_const_structs.h"

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

static BSEMAPHORE_DECL(rec_buffer_filled, true);
static BSEMAPHORE_DECL(fft_computed, true);

static THD_WORKING_AREA(selector_thd_wa, 2048);
static THD_WORKING_AREA(record_thd_wa, 2048);
static THD_WORKING_AREA(comm_thd_wa, 512);
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
uint id = 1;
char vmap[HEIGHT][WIDTH];
Coordinate * my_coordinate;


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

void broadcast(int number) {
    chThdSleepMilliseconds(100);
	dac_play((number * 200) + 1175);
    chThdSleepMilliseconds(700);
    dac_stop();
    chThdSleepMilliseconds(1500);

//	chThdSleepMilliseconds(100);
//	dac_play((number * 500) + 1250);
//	chThdSleepMilliseconds(700);
//	dac_stop();
//	chThdSleepMilliseconds(300);
}

int checkBroadcast(int number) {
	int rgb_from_freq;
	int time_first = ST2S(chVTGetSystemTimeX());
	int time_now = ST2S(chVTGetSystemTimeX());
	while((time_now - time_first) < number) {
		time_now = ST2S(chVTGetSystemTimeX());
		rgb_from_freq = listen();
		if(rgb_from_freq > 1000) {
			time_first = ST2S(chVTGetSystemTimeX());
		}
	}
	canBroadcast = 1;
	return 1;
}

//masalah: kalo robot 1 slesai, robot 2 broadcast kerja bersamaan.
static THD_FUNCTION(working_thd, arg) {
    (void) arg;
    chRegSetThreadName(__FUNCTION__);

    while(!is_complete(vmap)) {
    	if(!connectPhase) {
			//check map also
//    		 if (is_complete(vmap)) {
//				break;
//    		 }
    		 if(can_work()) {
				chThdSleepMilliseconds(500);

				set_rgb_led(LED2, 0, 0 ,1);
				Coordinate * nearestCoordinate;
				go_work();
				//ambil last di types.h
				nearestCoordinate = get_nearest_coordinate(vmap, my_coordinate -> x , my_coordinate -> y);


				//broadcast id and coordinate
				if(get_my_id() == 1)
					set_led(LED1, 1);
				if(checkBroadcast(2)) {
					broadcast(get_my_id());
//					broadcast(0);
//					broadcast((int)nearestCoordinate -> x/10);
//					broadcast((int)nearestCoordinate -> x%10);
					broadcast((int)nearestCoordinate -> x/4);
					broadcast((int)nearestCoordinate -> x%4);

//					broadcast(0);
//					broadcast((int)nearestCoordinate -> y/10);
//					broadcast((int)nearestCoordinate -> y%10);
					broadcast((int)nearestCoordinate -> y/4);
					broadcast((int)nearestCoordinate -> y%4);
				    canBroadcast = 0;
				    chThdSleepMilliseconds(100);
				    move_robot_in_map(vmap, get_my_id(), *my_coordinate, *nearestCoordinate);
					my_coordinate -> x = nearestCoordinate -> x;
					my_coordinate -> y =  nearestCoordinate -> y;

				}
				//move?
//				if(can_done()) {
//					if(checkBroadcast(3)) {
//						broadcast(get_my_id());
//						broadcast(1);
//						broadcast(11);
//						go_done();
//						chThdSleepMilliseconds(100);
//						canBroadcast = 0;
//						chThdSleepMilliseconds(100);
//					}
//				}

					set_rgb_led(LED2, 0, 0 ,0);
					set_rgb_led(LED6, 0, 0 ,0);
				while(1) {

//					if(can_free()) {
						set_rgb_led(LED4, 0, 1, 0);
						set_rgb_led(LED8, 1, 0, 0);
						if(checkBroadcast(5)) {
							set_rgb_led(LED4, 0, 1, 0);
							//this is broadcast for free.
							broadcast(get_my_id());
							broadcast(5);
//							broadcast(1);
							//change it to get_my_id
							robot_moved_in_map(vmap, get_my_id(), *my_coordinate);
//							go_free();
							push_to_free_robots_list(get_robot_with_index(get_my_id() - 1));
							chThdSleepMilliseconds(100);
						    canBroadcast = 0;
						    chThdSleepMilliseconds(100);


							//newly added
							get_robot_with_index(get_my_id() - 1).coordinate->x = nearestCoordinate -> x;
							get_robot_with_index(get_my_id() - 1).coordinate->y = nearestCoordinate -> y;


							break;
//						}
					}
//					else {
//						set_rgb_led(LED8, 0, 1, 0);
//					}
				}
			}
    	}
    	//very important!
    	else {
    		set_rgb_led(LED6, 0, 0 , 1);
    	}
    	set_rgb_led(LED6,0, 0, 0);
    }
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
	while((time_now - time_first) < 10) {

		rgb_from_freq = listen();
		set_rgb_led(LED2, 0, 0, 1);
		if (rgb_from_freq > 2000) {
			time_first = ST2S(chVTGetSystemTimeX());
			chThdSleepMilliseconds(1000);
			Coordinate * new_coordinate = (Coordinate*) malloc(sizeof(Coordinate));
			if (id == 1) {
				set_led(LED1, 1);
			}
			new_coordinate -> x = 1;
			new_coordinate -> y = id;
			new_robot(id, new_coordinate);
			setRobot(vmap, id, new_coordinate -> x, new_coordinate -> y);

			id++;
		} else {
			time_now = ST2S(chVTGetSystemTimeX());
		}
	}

	//adding this robot to the list
	set_led(LED1, 0);

    my_coordinate = (Coordinate *) malloc(sizeof(Coordinate));
    my_coordinate->x = 1;
    my_coordinate->y = id;
    new_robot(id, my_coordinate);
	setRobot(vmap, id, my_coordinate -> x, my_coordinate -> y);
	set_my_id(id);
	id++;
	set_rgb_led(LED2, 0, 0, 0);

	time_first = ST2S(chVTGetSystemTimeX());
	time_now = ST2S(chVTGetSystemTimeX());
	//waiting for the next robots
    while((time_now - time_first) < 10) {

		set_rgb_led(LED4, 0, 0, 1);
		rgb_from_freq = listen();
		if(rgb_from_freq > 2000) {
			//pake can work()
			chThdSleepMilliseconds(2000);
			chThdSleepMilliseconds(500);
			dac_play(2100);
			time_first = ST2S(chVTGetSystemTimeX()) + 10;
			chThdSleepMilliseconds(400);
			Coordinate * new_coordinate = (Coordinate*) malloc(sizeof(Coordinate));
			new_coordinate -> x = 1;
			new_coordinate -> y = id;
			new_robot(id, new_coordinate);
			setRobot(vmap, id, new_coordinate -> x, new_coordinate -> y);
			id++;
			dac_stop();
			chThdSleepMilliseconds(600);
		} else {
			time_now = ST2S(chVTGetSystemTimeX());
		}
    }
    set_rgb_led(LED4, 0, 0, 0);
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
	int sounds[6];
	int index = 0;

    while(!is_complete(vmap)) {
//		if(index % 2 == 0) {
//			set_led(LED3, 1);
//		} else {
//			set_led(LED3, 0);
//		}

    	if(!connectPhase && !canBroadcast) {

    		rgb_from_freq = listen();
    		set_rgb_led (LED2, 0, 0, 0);
    		set_led(LED5, 0);
    		set_led(LED1, 0);
    		set_led(LED7, 0);
    		set_led(LED3, 0);

			if(rgb_from_freq < 1000) {
	    		set_rgb_led(LED2, 1, 0, 0);
	    		continue;

			}

//	    	else if(rgb_from_freq < 1100) {
//				sounds[index%5] = 0;
//				set_rgb_led(LED2, 0, 1, 0);
//	    		index++;
//	    		chThdSleepMilliseconds(900);
//			}

			else if(rgb_from_freq < 1200) {
				sounds[index] = 0;
//				sounds[index%5] = 1;
	    		set_led(LED1, 1);
	    		index++;
	    		chThdSleepMilliseconds(900);
			}

//			else if(rgb_from_freq < 1300) {
//				sounds[index%5] = 2;
//				set_led(LED5, 1);
//	    		index++;
//	    		chThdSleepMilliseconds(900);
//			}

			else if(rgb_from_freq < 1400) {
				sounds[index] = 1 - 1;
//				sounds[index%5] = 3;
	    		set_led(LED3, 1);
	    		index++;
	    		chThdSleepMilliseconds(900);
			}

//			else if(rgb_from_freq < 1500) {
////				sounds[index%5] = 4;
//				sounds[index%5] = 0;
//				set_led(LED1, 1);
//	    		index++;
//	    		chThdSleepMilliseconds(900);
//			}

			else if(rgb_from_freq < 1600) {
				sounds[index] = 2 - 1;
//				sounds[index%5] = 5;
				set_led(LED5, 1);
	    		index++;
	    		chThdSleepMilliseconds(900);
			}

//			else if(rgb_from_freq < 1700) {
//				sounds[index%5] = 6;
//	    		index++;
//	    		chThdSleepMilliseconds(900);
//			}

			else if(rgb_from_freq < 1800) {
				sounds[index] = 3 - 1;
//				sounds[index%5] = 7;
				set_led(LED7, 1);
	    		index++;
	    		chThdSleepMilliseconds(900);
			}

//			else if(rgb_from_freq < 1900) {
//				sounds[index%5] = 8;
//	    		index++;
//	    		chThdSleepMilliseconds(900);
//			}

			else if(rgb_from_freq < 2000) {
//				sounds[index%5] = 1;
				sounds[index] = 4 - 1;
//				sounds[index%5] = 9;
				set_front_led(1);
	    		index++;
	    		chThdSleepMilliseconds(900);
	    		set_front_led(0);
			}

	    		//this is for the finish sound (> 2000)
			else {
				sounds[index] = 10;
	    		index++;
//				set_led(LED5, 1);
//	    		chThdSleepMilliseconds(600);
	    		chThdSleepMilliseconds(900);
			}

			//free
			if (index == 2) {
				int robot_id = sounds[0];
				if (sounds[1] == 10) {
					set_body_led(1);
					set_led(LED7,1);
					index = 0;
//					go_free();
					robot_moved_in_map(vmap, robot_id, *get_robot_with_index(robot_id - 1).coordinate);
					push_to_free_robots_list(get_robot_with_index(robot_id - 1));

					chThdSleepMilliseconds(20);
					chThdSleepMilliseconds(1000);
				}
			}

			//done
//			if (index % 5 == 2) {
//				if (sounds[2] == 10) {
//					set_led(LED7,1);
//					index = 0;
//					go_done();
//					chThdSleepMilliseconds(20);
//					chThdSleepMilliseconds(1000);
//				}
//			}

			set_led(LED7,0);
			//HOPE THERE IS NO NOISEE
			//updating from broadcast
			if(index ==5 ) {
				chThdSleepMilliseconds(2000);

				int robot_id = sounds[0];
				if (robot_id == 1) {
					set_led(LED1,1);
				}
				int pos_x = sounds[1] * 4 + sounds[2];
				int pos_y = sounds[3] * 4 + sounds[4];
//				if (pos_x == 2) {
//					set_front_led(1);
//				}
//				if (pos_y == 1) {
//					set_body_led(1);
//				}
//				if (pos_y == 0) {
//					left_motor_set_speed(100);
//				}
//				if (pos_y == 2) {
//					left_motor_set_speed(800);
//				}
//
//				if (pos_y == 3) {
//					turnLeft(500);
//				}
//
//				if (pos_y == 4) {
//					set_rgb_led(LED8, 0,0,1);
//				}
				//the robots has to pop another robot
				chThdSleepMilliseconds(600);
//				chThdSleepMilliseconds(5000);
//				chThdSleepMilliseconds(5000);
//				chThdSleepMilliseconds(5000);
				go_work();
				Coordinate * new_coordinate = (Coordinate*) malloc(sizeof(Coordinate));
				new_coordinate -> x =  pos_x;
				new_coordinate -> y = pos_y;
			    move_robot_in_map(vmap, robot_id, *get_robot_with_index(robot_id - 1).coordinate, *new_coordinate);
				index = 0;

				get_robot_with_index(robot_id - 1).coordinate -> x = pos_x;
				get_robot_with_index(robot_id - 1).coordinate -> y = pos_y;
				//test first. afraid there are delays
				// TODO : update map
			}
			if (index == 5) {
				index =0;
			}
		}
    	else {
    		set_rgb_led(LED8, 1,0,0);
    	}
    	set_rgb_led(LED8, 0 ,0 ,0);
    }
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
	proximity_start();
    mic_start(mic_callback);
    usb_start();
    dac_start();
    spi_comm_start();
    VL53L0X_start();
    sdio_start();
    playSoundFileStart();
    playMelodyStart();
    motors_init();

    construct_map(vmap);
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
