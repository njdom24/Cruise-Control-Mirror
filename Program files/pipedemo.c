#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/select.h>
#include <string.h>

#include "datastore.h"
#include "currentstate.h"

int main(int argc, char** argv) {
    fd_set set;
    int fd[2];//interface to currentstate?
    int fd2[2];//currentstate to datastore?

    if(pipe(fd) < 0)
        exit(1);
    if(pipe(fd2) < 0)
        exit(1);

    FD_ZERO(&set);
    FD_SET(fd[0], &set);
    FD_SET(fd2[0], &set);
    
    char read_str[100];

    start_datastore(fd);
    start_curstate(fd2);
    /*double set_speed;
    double current_speed;
    double *buf=calloc(sizeof(double), 2);
    if(read(fd2[0], buf, sizeof(double)*2+2) >0) {
        set_speed = buf[1];
        current_speed = buf[0];
        if(set_speed <= 110 && set_speed >= 20) {
            current_speed=set_speed;
            fd2[1]=fd2[0];
            fprintf(stdout,"User changed set speed to %lf\n", current_speed);
        }
        else {
            write(STDERR_FILENO, "WARNING: Cannot initiate CC outside the range of 20mph to 110mph\n", 66);
            write(STDOUT_FILENO,"User attempted to initiate CC outside of permitted range\n", 58);
        }
    }*/
    if(close(fd[1]) == -1) {
        fprintf(stderr, "Error: close failed.\n");
    }
    if(close(fd2[1]) == -1) {
        fprintf(stderr, "Error: close failed.\n");
    }
    while(true) {
        fd_set temp = set;
        int ret = select(FD_SETSIZE, &temp, NULL, NULL, NULL);

        //No activity on pipe
        if(ret == 0) {
            
        }
        //Pipe error
        else if (ret < 0) {
            perror("Pipe error");
        }
        //Activity on the pipe
        else {
            for(int i = 0; i < FD_SETSIZE; i++) {
                if(FD_ISSET(i, &temp)) {
                    read(i, read_str, 100);
                    printf("%d: %s\n", i, read_str);
                    memset(read_str,0,strlen(read_str));
                }
            }

        }
    }
    return 0;
}