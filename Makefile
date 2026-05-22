CC     = gcc
CFLAGS = -O2 -Wall

maze: maze.c
	$(CC) $(CFLAGS) maze.c -o maze

run: maze
	./maze

clean:
	rm -f maze

.PHONY: run clean
