CC=gcc
CFLAGS=-I.

all: sequental_min_max parallel_min_max run_sequental

sequental_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o sequental_min_max find_min_max.o utils.o sequental_min_max.c $(CFLAGS)

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

run_sequental: run_sequental.c
	$(CC) -o run_sequental run_sequental.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

clean :
	rm utils.o find_min_max.o sequental_min_max parallel_min_max run_sequental
