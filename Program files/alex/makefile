testhead: functions.o main.o
	gcc functions.o main.o -o testhead
functions.o : main.c
	gcc -g -Wall -Werror -pedantic-errors -c main.c
main.o: functions.c functions.h
	gcc -g -Wall -Werror -pedantic-errors -c functions.c
clean:
	rm -f *.o testhead testhead.exe
#.exe not needed on linux only siguine