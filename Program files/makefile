CC    = gcc
FLAGS =  -Wall -Werror -Wno-unused-variable -pedantic-errors

.PHONY: all clean

all: pipedemo
pipedemo: pipedemo.o datastore.o currentstate.o
	$(CC) -g pipedemo.o datastore.o currentstate.o -o pipedemo
pipedemo.o : pipedemo.c
	$(CC) -g $(FLAGS) -c pipedemo.c
datastore.o: datastore.c datastore.h
	$(CC) -g $(FLAGS) -c datastore.c
currentstate.o: currentstate.c currentstate.h
	$(CC) -g $(FLAGS) -c currentstate.c
clean:
	rm -f *.o pipedemo pipedemo.exe
