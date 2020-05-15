window.out: backend.o interface.o
ifeq ($(OS),Windows_NT) 
	gcc interface.o backend.o -o window.exe `pkg-config --cflags --libs gtk+-3.0` -mwindows
else
	gcc interface.o backend.o -o window.out `pkg-config --cflags --libs gtk+-3.0`
endif

backend.o: backend.c
	gcc backend.c -c `pkg-config --cflags --libs gtk+-3.0`

interface.o: backend.o interface.c
	gcc interface.c backend.h -c `pkg-config --cflags --libs gtk+-3.0` 

clean:
	rm *.o window.exe window.out
