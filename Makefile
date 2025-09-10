# Compilatore e flags
CC = gcc
CFLAGS = -Wall -ggdb -std=c23 -pedantic $(shell pkg-config --cflags sdl3)
LIBS = $(shell pkg-config --libs sdl3)

# File sorgenti e oggetti
SRCS = main.c game.c render.c
OBJS = $(SRCS:.c=.o)

# Nome eseguibile
TARGET = main

# Compilazione dell'eseguibile
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Compilazione dei singoli oggetti
main.o: main.c render.h
	$(CC) $(CFLAGS) -c $< -o $@
game.o: game.c game.h
	$(CC) $(CFLAGS) -c $< -o $@
render.o: render.c game.h render.h
	$(CC) $(CFLAGS) -c $< -o $@

# Pulizia
clean:
	rm -f $(OBJS) $(TARGET)

# Pulizia completa (anche file temporanei, se ci fossero)
distclean: clean