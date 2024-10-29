CC = gcc

CFLAGS = -Wall -Wextra -I.

TARGETS = parallel_min_max process_memory parallel_sum
OBJS = parallel_min_max.o find_min_max.o utils.o sum.o process_memory.o

all: $(TARGETS)

parallel_min_max: parallel_min_max.o find_min_max.o utils.o
	$(CC) $(CFLAGS) -o $@ $^

process_memory: process_memory.o
	$(CC) $(CFLAGS) -o $@ $^

parallel_sum: parallel_sum.o utils.o sum.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

parallel_min_max.o: parallel_min_max.c utils.h find_min_max.h
	$(CC) $(CFLAGS) -c parallel_min_max.c

process_memory.o: process_memory.c
	$(CC) $(CFLAGS) -c process_memory.c

parallel_sum.o: parallel_sum.c utils.h sum.h
	$(CC) $(CFLAGS) -c parallel_sum.c

find_min_max.o: find_min_max.c utils.h find_min_max.h
	$(CC) $(CFLAGS) -c find_min_max.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

sum.o: sum.c sum.h
	$(CC) $(CFLAGS) -c sum.c

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean
