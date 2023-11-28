CC := cc
CFLAGS := -Wall -Wextra -Werror -std=c99 -O2
SRC := $(wildcard *.c)
TARGET := $(SRC:.c=)

build: $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
