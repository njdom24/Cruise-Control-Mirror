window.out: interface.o
ifeq ($(OS),Windows_NT) 
	gcc interface.o -o window.exe `pkg-config --cflags --libs gtk+-3.0` -mwindows
else
	gcc interface.o -o window.out `pkg-config --cflags --libs gtk+-3.0`
endif

interface.o: interface.c
	gcc interface.c -c `pkg-config --cflags --libs gtk+-3.0` -static

clean:
	rm *.o window.exe window.out
