#include "currentstate.h"
#include <string.h>
#include <stdbool.h>

int start_curstate(int fd[2]) {
    int count = 0;
    if (fork() == 0) {
        while(true) {
            char input_str[] = "Hello from state pipe!";
            write(fd[1], input_str, strlen(input_str));
            sleep(3);
        }
        exit(0);
    }
    return 1;
}