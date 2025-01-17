CC = gcc
CFLAGS = -O0 -Wall
LDFLAGS = -lm

DELAY=750

all: work1 work2 work3 work4


work1: dummy_stuff.c
	$(CC) $(CFLAGS) -DLOAD=1 -DDELAY=$(DELAY) -o work1 dummy_stuff.c

work2: print_1_1000.c
	$(CC) $(CFLAGS) -DLOAD=2 -DDELAY=$(DELAY) -o work2 print_1_1000.c

work3: print_1001_2000.c
	$(CC) $(CFLAGS) -DLOAD=3 -DDELAY=$(DELAY) -o work3 print_1001_2000.c
work4: dummy_stuff.c
	$(CC) $(CFLAGS) -DLOAD=4 -DDELAY=$(DELAY) -o work4 dummy_stuff.c


clean:
	rm -f work[1-4]