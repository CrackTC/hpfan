CC := cc
CFLAGS := -Wall -Wextra -Werror -std=c99 -O2
SRC := $(wildcard *.c)
TARGET := $(SRC:.c=)
SIGNATURE := $(TARGET).sig

clean:
	rm -f $(TARGET) $(SIGNATURE)

build: $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

sign: clean build
	gpg --detach-sign $(TARGET)
