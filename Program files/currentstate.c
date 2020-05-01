#include "currentstate.h"
#include <string.h>
#include <stdbool.h>

int start_curstate(int fd[2]) {
    int count = 0;

    if (fork() == 0) {
        while(true) {
            /*within the child process we will be reading the current speed and
            * the set speed from the speed.txt file
            * the input within will be formatted as a directory alternating between current then set
            * so we take data here then send it to the main logic? and then if the speeds do not match
            * (if CC-State is on then we need to call 
            * "gtk_adjustment_set_value (GTK_ADJUSTMENT (speed_slider), ss);" in)
            */
            char path[_PC_PATH_MAX];
            if (realpath("./User_Interface/speeds.txt", path) == NULL) {/*something is bad*/}
            FILE *spd=fopen(path, "r");
            double cspeed;
            double sspeed;
            int track = 0;
            char* speeds=malloc(sizeof(double)+1);
            while(fgets(speeds, sizeof(double)+1, spd) < 0) {
                for(int i=0;i < sizeof(double)+1; i++) {
                    if(speeds[i] == '\0') {break;}
                    else if(speeds[i] == '\n') {
                        speeds[i] = '\0';
                        break;
                    }
                }
                if(track % 2==0) {cspeed=atof(speeds);}
                else {sspeed=atof(speeds);}
                track++;
                free(speeds);
                speeds = malloc(sizeof(double)+1);
            }
            printf("%lf\n%lf\n", cspeed, sspeed);//you've set up the pipes to send this to datastore right??
            fclose(spd);
            sleep(3);
        }
        exit(0);
    }
    return 1;
}