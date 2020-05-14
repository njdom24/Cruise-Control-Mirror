#include "functions.h"

/*static double initiate_CC(double currentspeed);
static bool deactivate_CC();
static double change_CC_set_speed(double usr_set_speed);
static void hit_the_brakes();*/


double initiate_CC(double currentspeed) {
	//NOTE: returns -1 on a failure to initiate cruising and the set speed on Success
	if(state > 0) {
		fprintf(stderr, "ALERT: You're already Cruising\n");//alternatively call deactivate and return 0
		//write(LOG_FILENO, "User attempted to initiate activated CC\n", 41);
		return -1;
	}
	if(currentspeed < 20) {
		fprintf(stderr, "ALERT: Driver cannot initiate cruise control below 20 mph\n");
		//write(LOG_FILENO, "User attempted to initiate CC outside the range of 20mph to 110mph\n", 68);
		//TODO add in the current speed to the log
		return -1;
	} else if(currentspeed > 110) {
		fprintf(stderr, "ALERT: Driver cannot initiate cruise control above 110 mph\n");
		//write(LOG_FILENO, "User attempted to initiate CC outside the range of 20mph to 110mph\n", 68);
		//TODO add in the current speed to the log
		return -1;
	}
	printf("Begin Cruising\n");
	//write(LOG_FILENO, "User attempted to deactivate uninitiated or idle CC\n", 53);
	state = 1;
	set_speed = currentspeed;
	return currentspeed;
}
void gas_pedal() {
	printf("CC is stalling while you are accelerating\n");
	state = 2;
}
bool deactivate_CC() {
	//NOTE: returns true if successfully set state from active/stalling to deactivated, otherwise returns false
	if(state == 0) {
		fprintf(stderr, "ALERT: You are not Cruising\n");//alternatively call initiate and return true
		//write(LOG_FILENO, "User attempted to deactivate uninitiated or idle CC\n", 53);
		return false;
	}
	printf("End Cruising\n");
	//write(LOG_FILENO, "Deactivating CC\n", 17);
	state = 0;
	set_speed = 0;
	return true;
}
double change_CC_set_speed(double usr_set_speed) {
	//NOTE: returns -1 on a failure to initiate cruising and the set speed on Success
	if(usr_set_speed < 20) {
		fprintf(stderr, "ALERT: Driver cannot set cruise control speed below 20 mph\n");
		//write(LOG_FILENO, "User attempted to initiate CC outside the range of 20mph to 110mph\n", 69);
		//TODO add in the current speed to the log
		return -1;
	} else if(usr_set_speed > 110) {
		fprintf(stderr, "ALERT: Driver cannot set cruise control speed above 110 mph\n");
		//write(LOG_FILENO, "User attempted to set CC speed outside the range of 20mph to 110mph\n", 69);
		//TODO add in the current speed to the log
		return -1;
	}
	printf("Cruising at %f mph\n", usr_set_speed);
	//write(LOG_FILENO, "User attempted to deactivate uninitiated or idle CC\n", 53);
	state = 1;
	set_speed = usr_set_speed;
	return usr_set_speed;
}
void hit_the_brakes() {
	printf("Braking detected, deactivating CC\n");
	//write(LOG_FILENO, "Braking detected, deactivating CC\n", 35);
	deactivate_CC();
}
