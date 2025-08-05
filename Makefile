CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra
LDFLAGS=

BIN_DIR = bin
TARGET = $(BIN_DIR)/ezns

SRCS = $(wildcard *.c)

all: $(TARGET)

$(TARGET): $(SRCS)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf bin
