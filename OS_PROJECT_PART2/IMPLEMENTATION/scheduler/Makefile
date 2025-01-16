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
