	#include <limits.h>
	#include <math.h>
	#include <stdbool.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
	#include "functions.h"

int main() {
	double currentspd = 0;
	char buf[580];
	int i;
	char* tmp;
	do {
		if(state == 1 && set_speed!=currentspd) {
			currentspd=set_speed;
		}
		printf("Current speed is %f\n\t", currentspd);
		fflush(stdout); 
		ssize_t bytes_read = read(STDIN_FILENO, buf, 579);
		
		if(bytes_read > 0) {
			buf[bytes_read - 1] = '\0';
		}
		if(strncmp(buf, "exit", 4) == 0) {
			break;
		}
		if(strncmp(buf, "brake", 5) == 0) {
			if(state < 1)
				currentspd -=10;
			else
				hit_the_brakes();
		}
		if(strncmp(buf, "gas", 3) == 0) {
			currentspd +=10;
			if(state >0)
				gas();
		} else if(state == 2) {
			state--;
		}
		if(strncmp(buf, "cc speed=", 9) == 0) {
			tmp = (char*) malloc((bytes_read-9)*sizeof(char));
			for(i = 10; buf[i] != '\0' && buf[i] != '\n' && (i-10) <= (int)log10(__DBL_MAX__); i++)
				*(tmp+(i-10)) = buf[i];
			*(tmp+i)='\0';
			currentspd = change_CC_set_speed(atof(tmp));
			free(tmp);
		}
		if(strncmp(buf, "cc on", 5) == 0) {
			currentspd = initiate_CC(currentspd);
		}
		if(strncmp(buf, "cc off", 6) == 0) {
			deactivate_CC();
		}
		if(currentspd >= 2) {
			currentspd -=2;
		}
		buf[0]='\0';
	} while (true);
	return EXIT_SUCCESS;
}
