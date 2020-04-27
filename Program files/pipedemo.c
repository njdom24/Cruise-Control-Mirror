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
    int fd[2];
    int fd2[2];

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
    if(atoi(((char*)fd2[1])[0]) > 110 || atoi(((char*)fd2[1])[0]) < 20) {
        fd2[1]=fd2[0];
        //!!this may be the wrong way to change the contents of the fd
        fprintf(stdout, "WARNING: Cannot initiate CC outside the range of 20mph to 110mph\n");
        //!!later replace stdout with file to alert the user
    }
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