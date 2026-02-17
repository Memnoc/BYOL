CC=gcc
CFLAGS=-Wall -g

all: parsing.c
	$(CC) $(CFLAGS) parsing.c mpc.c lib/linenoise.c -o parsing -lm

doge: examples/doge_example.c
	$(CC) $(CFLAGS) examples/doge_example.c mpc.c -o doge -lm
