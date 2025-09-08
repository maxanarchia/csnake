CC=gcc
CFLAGS=-Wall -ggdb -std=c23 -pedantic $(shell pkg-config --cflags sdl3)
LIBS=$(shell pkg-config --libs sdl3)

main: main.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
