CC     = gcc
CFLAGS = -O3 -Wall
SDL    = $(shell sdl2-config --cflags --libs)

maze: maze.c
	$(CC) $(CFLAGS) maze.c -o maze $(SDL)

run: maze
	./maze

clean:
	rm -f maze

.PHONY: run clean
