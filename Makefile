CC=gcc
CFLAGS=-Wall -lncurses

build: tetris

tetris: main.c
	$(CC) $(CFLAGS) main.c -o tetris

run: tetris
	./tetris
	
clean: tetris
	rm tetris
