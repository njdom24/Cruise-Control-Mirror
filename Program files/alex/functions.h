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

#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/select.h>
#include <string.h>

int state;
double set_speed;

//#define LOG_FILENO 10

double initiate_CC(double currentspeed);
bool deactivate_CC();
double change_CC_set_speed(double usr_set_speed);
void hit_the_brakes();