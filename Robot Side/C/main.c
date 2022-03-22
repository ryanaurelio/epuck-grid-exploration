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

static THD_WORKING_AREA(proximity_thd_wa, 512);
static THD_WORKING_AREA(selector_thd_wa, 2048);
static THD_WORKING_AREA(record_thd_wa, 2048);
static THD_WORKING_AREA(comm_thd_wa, 512);
static THD_WORKING_AREA(move_thd_wa, 512);
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
int time_move = 0;
int bisa_maju = 1;


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

void broadcast_done() {
    chThdSleepMilliseconds(100);
	dac_play(2100);
    chThdSleepMilliseconds(700);
    dac_stop();
    //TODO trying to make the time smaller later
    chThdSleepMilliseconds(1500);
}


//void broadcast(int x, int y, int num) {
//	int numberBroadcast[6];
//	numberBroadcast[0] = get_my_id();
//	numberBroadcast[1] = x/4;
//	numberBroadcast[2] = x%4;
//	numberBroadcast[3] = y/4;
//	numberBroadcast[4] = y%4;
//	numberBroadcast[5] = num;
//	for(int i = 0; i < 6; i++) {
//	    chThdSleepMilliseconds(100);
//		dac_play((numberBroadcast[i] * 200) + 1175);
//	    chThdSleepMilliseconds(700);
//	    dac_stop();
//	    //TODO trying to make the time smaller later
//	    chThdSleepMilliseconds(1500);
//	}
//}


void broadcast(int number) {
    chThdSleepMilliseconds(50);
	dac_play((number * 200) + 1175);
//    chThdSleepMilliseconds(700);
	chThdSleepMilliseconds(200);
    dac_stop();
    //TODO trying to make the time smaller later CHANGE LAST HERE
    chThdSleepMilliseconds(350);

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

//void check_wall(void) {
//	calibrate_ir();
//	int num_left = get_prox(2);
//	int num_right = get_prox(5);
//	int num_front1 = get_prox(0);
//	int num_front2 = get_prox(7);
//	int num_back1 = get_prox(3);
//	int num_back2 = get_prox(4);
////		uint16_t temp_dist = VL53L0X_get_dist_mm();
////		uint16_t val = 75;
//	//tof doesnt work?
//
//	//can't find a good value for proximity. moving and getting proximity together makes some noises on the proximity value.
//
//
//	int pos_x = get_robot_with_index(get_my_id() - 1) -> coordinate -> x;
//	int pos_y = get_robot_with_index(get_my_id() - 1) -> coordinate -> y;
//	direction direction = get_robot_with_index(get_my_id() - 1) -> direction;
//
//	//Approximately 1.75 cm
//	if(num_left > 200) {
//		//0 is blocked
//		switch(direction) {
//		case UP:
//			broadcast(pos_x - 1,  pos_y, 0);
//			break;
//		case DOWN:
//			broadcast(pos_x + 1,  pos_y, 0);
//			break;
//		case LEFT:
//			broadcast(pos_x,  pos_y + 1, 0);
//			break;
//		case RIGHT:
//			broadcast(pos_x - 1,  pos_y - 1, 0);
//			break;
//		}
//	}
//
//	//Approximately 1.75 cm
//	if (num_right > 200) {
//		//0 is blocked
//		switch(direction) {
//		case UP:
//			broadcast(pos_x + 1,  pos_y, 0);
//			break;
//		case DOWN:
//			broadcast(pos_x - 1,  pos_y, 0);
//			break;
//		case LEFT:
//			broadcast(pos_x,  pos_y - 1, 0);
//			break;
//		case RIGHT:
//			broadcast(pos_x - 1,  pos_y + 1, 0);
//			break;
//		}
//	}
//
//	//have to check
//	if ((num_front1 + num_front2)/2 >200) {
//		//0 is blocked
//		switch(direction) {
//		case UP:
//			broadcast(pos_x,  pos_y - 1, 0);
//			break;
//		case DOWN:
//			broadcast(pos_x,  pos_y + 1, 0);
//			break;
//		case LEFT:
//			broadcast(pos_x - 1,  pos_y, 0);
//			break;
//		case RIGHT:
//			broadcast(pos_x + 1,  pos_y, 0);
//			break;
//		}
//	}
//
//	//have to check
//	if ((num_back1 + num_back2)/2 >200) {
//		//0 is blocked
//		switch(direction) {
//		case UP:
//			broadcast(pos_x,  pos_y - 1, 0);
//			break;
//		case DOWN:
//			broadcast(pos_x,  pos_y + 1, 0);
//			break;
//		case LEFT:
//			broadcast(pos_x - 1,  pos_y, 0);
//			break;
//		case RIGHT:
//			broadcast(pos_x + 1,  pos_y, 0);
//			break;
//		}
//	}
//
//}


static THD_FUNCTION(working_thd, arg) {
    (void) arg;
    chRegSetThreadName(__FUNCTION__);

    while(!is_complete(vmap)) {
    	if(!connectPhase) {

    		 if(can_work()) {
    			 //adding this
//    			 if(checkBroadcast(3)) {
//    				 check_wall();
//    				 canBroadcast = 0;
//    			 }

				chThdSleepMilliseconds(500);
				set_rgb_led(LED2, 0, 0 ,1);
				Coordinate * nearestCoordinate;
				go_work();

				nearestCoordinate = get_nearest_coordinate(vmap, get_robot_with_index(get_my_id() - 1) -> coordinate -> x ,
						get_robot_with_index(get_my_id() - 1) -> coordinate -> y);

				if(checkBroadcast(1)) {
					//adding this
					//3 means robot move
//					broadcast((int)nearestCoordinate -> x, (int)nearestCoordinate -> y, 3);


					//removing this
					broadcast(get_my_id());
					broadcast((int)nearestCoordinate -> x/3);
					broadcast((int)nearestCoordinate -> x%3);
					broadcast((int)nearestCoordinate -> y/3);
					broadcast((int)nearestCoordinate -> y%3);

				    canBroadcast = 0;
				    chThdSleepMilliseconds(100);

				    move_robot_in_map(vmap, get_my_id(), *get_robot_with_index(get_my_id() - 1) -> coordinate, *nearestCoordinate);

					get_robot_with_index(get_my_id() - 1) -> coordinate->x = nearestCoordinate -> x;
					get_robot_with_index(get_my_id() - 1) -> coordinate->y = nearestCoordinate -> y;

				}

				while(1) {

					if(checkBroadcast(3 + (0.2 * get_my_id()))) {
						set_rgb_led(LED4, 0, 1, 0);
						//this is broadcast for free.
						//adding this
//						broadcast_done();

						//removing this
						broadcast(get_my_id());
						broadcast(4);

						robot_moved_in_map(vmap, get_my_id(), *get_robot_with_index(get_my_id() - 1) -> coordinate);

						push_to_free_robots_list(get_robot_with_index(get_my_id() - 1));

						canBroadcast = 0;

						set_rgb_led(LED4, 0, 0, 0);

						break;
					}
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
	while((time_now - time_first) < 5) {

		rgb_from_freq = listen();
		set_rgb_led(LED2, 0, 0, 1);
		if (rgb_from_freq > 2000) {
			time_first = ST2S(chVTGetSystemTimeX());
			set_led(LED1, 1);
			chThdSleepMilliseconds(1000);
			Coordinate * new_coordinate = (Coordinate*) malloc(sizeof(Coordinate));

			new_coordinate -> x = 1;
			new_coordinate -> y = id;
			new_robot(id, new_coordinate);
			setRobot(vmap, id, new_coordinate -> x, new_coordinate -> y);
			id++;

			set_led(LED1, 0);

		} else {
			time_now = ST2S(chVTGetSystemTimeX());
		}
	}

	Coordinate * my_coordinate;
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
	int sounds[6];
	int index = 0;

    while(!is_complete(vmap)) {

    	if(!connectPhase && !canBroadcast) {

    		rgb_from_freq = listen();

    		set_led(LED5, 0);
    		set_led(LED1, 0);
    		set_led(LED7, 0);
    		set_led(LED3, 0);

			if( rgb_from_freq > 1200 && rgb_from_freq < 1400) {
				sounds[index] = 0;
	    		set_led(LED1, 1);
	    		index++;
//	    		chThdSleepMilliseconds(900);
				chThdSleepMilliseconds(425);
			}

			else if(rgb_from_freq >= 1400 && rgb_from_freq < 1600) {
				sounds[index] = 1;
				set_led(LED3, 1);
	    		index++;
//	    		chThdSleepMilliseconds(900);
				chThdSleepMilliseconds(425);
			}

			else if(rgb_from_freq >= 1600 && rgb_from_freq < 1800) {
				sounds[index] = 2;
				set_led(LED5, 1);
	    		index++;
//	    		chThdSleepMilliseconds(900);
				chThdSleepMilliseconds(425);
			}

			else if(rgb_from_freq >= 1800 && rgb_from_freq < 2000) {
//				sounds[index] = 3;
//				set_led(LED7, 1);
//	    		index++;
//	    		chThdSleepMilliseconds(900);
				sounds[index] = 10;
	    		index++;
//	    		chThdSleepMilliseconds(900);
				chThdSleepMilliseconds(425);
			}

	    		//this is for the finish sound (> 2000)
			else if (rgb_from_freq >= 2000 && rgb_from_freq < 2200){
				sounds[index] = 10;
	    		index++;
//	    		chThdSleepMilliseconds(900);
				chThdSleepMilliseconds(425);
			}

			//done
			if (index == 2) {
				int robot_id = sounds[0];
				if (sounds[1] == 10) {
					set_body_led(1);
					index = 0;
					robot_moved_in_map(vmap, robot_id, *get_robot_with_index(robot_id - 1) -> coordinate);
					push_to_free_robots_list(get_robot_with_index(robot_id - 1));

					chThdSleepMilliseconds(120);
//					chThdSleepMilliseconds(1000);

					set_body_led(0);
				}
			}

			//updating from broadcast
			if(index == 5) {

				int robot_id = sounds[0];

				int pos_x = sounds[1] * 3 + sounds[2];
				int pos_y = sounds[3] * 3 + sounds[4];
//				int indicator = sounds[6];

//				if (indicator == 3) {
					go_work();
					Coordinate * new_coordinate = (Coordinate*) malloc(sizeof(Coordinate));
					new_coordinate -> x = pos_x;
					new_coordinate -> y = pos_y;
					move_robot_in_map(vmap, robot_id, *get_robot_with_index(robot_id - 1) -> coordinate, *new_coordinate);
					index = 0;

					get_robot_with_index(robot_id - 1) -> coordinate -> x = pos_x;
					get_robot_with_index(robot_id - 1) -> coordinate -> y = pos_y;
//				}
//				if (indicator == 0) {
//					//changing the unknown to a wall.
//				}
			}
			if (index == 5) {
				index = 0;
			}
		}
    	//this shows it's not listening.
    	else {
    		set_rgb_led(LED8, 1, 0, 0);
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



//    chThdCreateStatic(move_thd_wa, sizeof(move_thd_wa), NORMALPRIO, move_thd, NULL);
//    chThdCreateStatic(proximity_thd_wa, sizeof(proximity_thd_wa), NORMALPRIO, proximity_thd, NULL);



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
