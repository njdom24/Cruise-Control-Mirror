#include "currentstate.h"
#include <string.h>
#include <stdbool.h>

int start_curstate(int fd[2]) {
    int count = 0;

    if (fork() == 0) {
        while(true) {
            char *speeds = calloc(sizeof(int), 2);//keeps the set cruise control speed and current speed in a string
            for(int index = 0; index < 2 && read(fd[0], speeds, sizeof(int)) >= 0; index++) {
                //
            }
            write(fd[1], speeds, strlen(speeds));
            free(speeds);
            sleep(3);
        }
        exit(0);
    }
    return 1;
}