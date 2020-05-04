/*
* when the user tries to...
* -> change the set_speed have a function that...
* --> makes sure that CC is activated and alerts if it isn't
* --> makes sure that the usr_set_speed is within range and alerts if it isn't
* --> changes the set speed to usr_set_speed and returns it
* -> brake
* --> alerts the user changes set speed to 0 and deactivates CC
* -> activate CC
* --> makes sure that the current speed speed is within range and alerts if it isn't
*/

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/select.h>
#include <string.h>

bool state;
double set_speed;

//#define LOG_FILENO 10

double initiate_CC(double current_speed);
bool deactivate_CC();
double change_CC_set_speed(double usr_set_speed);

double initiate_CC(double current_speed) {
	//NOTE: returns -1 on a failure to initiate cruising and the set speed on Success
	if(state) {
		fprintf(stderr, "ALERT: You're already Cruising\n");//alternatively call deactivate and return 0
		//write(LOG_FILENO, "User attempted to initiate activated CC\n", 41);
		return -1;
	}
	if(current_speed < 20) {
		fprintf(stderr, "ALERT: Driver cannot initiate cruise control below 20 mph\n");
		//write(LOG_FILENO, "User attempted to initiate CC outside the range of 20mph to 110mph\n", 68);
		//TODO add in the current speed to the log
		return -1;
	} else if(current_speed > 110) {
		fprintf(stderr, "ALERT: Driver cannot initiate cruise control above 110 mph\n");
		//write(LOG_FILENO, "User attempted to initiate CC outside the range of 20mph to 110mph\n", 68);
		//TODO add in the current speed to the log
		return -1;
	}
	printf("Begin Cruising\n");
	//write(LOG_FILENO, "User attempted to deactivate uninitiated or idle CC\n", 53);
	state = true;
	set_speed = current_speed;
	return current_speed;
}
bool deactivate_CC() {
	//NOTE: returns true if successfully set state from true to false, otherwise returns false
	if(!state) {
		fprintf(stderr, "ALERT: You are not Cruising\n");//alternatively call initiate and return true
		//write(LOG_FILENO, "User attempted to deactivate uninitiated or idle CC\n", 53);
		return state;
	}
	state = false;
	set_speed = 0;
	return !state;
}
double change_CC_set_speed(double usr_set_speed) {//TODO: touch up the write statements
	//NOTE: returns -1 on a failure to initiate cruising and the set speed on Success
	if(state) {
		fprintf(stderr, "ALERT: You're already Cruising\n");//alternatively call deactivate and return 0
		//write(LOG_FILENO, "User attempted to initiate activated CC\n", 41);
		return -1;
	}
	if(current_speed < 20) {
		fprintf(stderr, "ALERT: Driver cannot initiate cruise control below 20 mph\n");
		//write(LOG_FILENO, "User attempted to initiate CC outside the range of 20mph to 110mph\n", 68);
		//TODO add in the current speed to the log
		return -1;
	} else if(current_speed > 110) {
		fprintf(stderr, "ALERT: Driver cannot initiate cruise control above 110 mph\n");
		//write(LOG_FILENO, "User attempted to initiate CC outside the range of 20mph to 110mph\n", 68);
		//TODO add in the current speed to the log
		return -1;
	}
	printf("Begin Cruising\n");
	//write(LOG_FILENO, "User attempted to deactivate uninitiated or idle CC\n", 53);
	state = true;
	set_speed = current_speed;
	return current_speed;
}

#endif