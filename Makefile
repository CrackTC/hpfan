CC := cc
CFLAGS := -Wall -Wextra -Werror -std=gnu99 -O2
SRC := $(wildcard *.c)
TARGET := $(SRC:.c=)
SIGNATURE := $(TARGET).sig

clean:
	rm -f $(TARGET) $(SIGNATURE)

build: $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

sign: $(TARGET)
	gpg --detach-sign $(TARGET)
