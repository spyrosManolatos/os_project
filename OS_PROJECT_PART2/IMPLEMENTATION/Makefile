CC = gcc
CFLAGS = -Wall
LDFLAGS = -lm

TARGET = exercise1
MAIN_SRC = exercise1.c
TEST_SRC = dummy_stuff.c print_1_1000.c print_1001_2000.c # delay file?
EXEC = $(TEST_SRC:.c=)

all: $(TARGET) $(EXEC)

$(TARGET): $(MAIN_SRC:.c=.o)
	$(CC) $(MAIN_SRC:.c=.o) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_EXEC): %: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(MAIN_SRC:.c=.o) $(EXEC) $(TEST_SRC:.c=.o)


