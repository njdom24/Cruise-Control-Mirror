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
                    if(close(i) == -1) {
                        fprintf(stderr, "Error: close failed. %s.\n", strerror(errno));
		                return EXIT_FAILURE;
                    }
                }
            }

        }
    }


    return 0;
}