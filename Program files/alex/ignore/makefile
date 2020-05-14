window.out: interface.o functions.o
ifeq ($(OS),Windows_NT) 
	gcc -g interface.o functions.o -o window.exe `pkg-config --cflags --libs gtk+-3.0` -mwindows
else
	gcc -g interface.o functions.o -o window.out `pkg-config --cflags --libs gtk+-3.0`
endif

interface.o: interface.c
	gcc interface.c -c `pkg-config --cflags --libs gtk+-3.0` -static
functions.o: functions.c functions.h
	gcc functions.c -c `pkg-config --cflags --libs gtk+-3.0` -static

clean:
	rm *.o window.exe window.out
