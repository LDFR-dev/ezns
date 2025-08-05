CC=gcc
CFLAGS=-Wall -Wextra
LDFLAGS=

all: bin/ezns

bin/ezns: ezns.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -rf bin
