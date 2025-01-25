CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -lm

all: exercise1 exercise2

exercise1: exercise1.c
	$(CC) $(CFLAGS) -o exercise1 exercise1.c $(LDFLAGS)

exercise2: exercise2.c
	$(CC) $(CFLAGS) -o exercise2 exercise2.c $(LDFLAGS)

clean:
	rm -f exercise1 exercise2
	rm -f fcfs_exercise1.txt
	rm -f rrff_exercise1.txt
	rm -f fcfs_exercise2.txt
	rm -f rr_exercise1.txt
