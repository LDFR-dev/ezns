CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Iinclude
LDFLAGS=

BIN_DIR = bin
TARGET = $(BIN_DIR)/ezns

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf bin $(wildcard src/*.o)