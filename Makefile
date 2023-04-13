CC=gcc
CFLAGS=-Wall -lncurses

build: tetris

tetris: bin/main.o bin/render.o
	$(CC) $(CFLAGS) -o tetris bin/main.o bin/render.o

bin/main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o bin/main.o

bin/render.o: src/render.c
	$(CC) $(CFLAGS) -c src/render.c -o bin/render.o

run: tetris
	./tetris
	
clean:
	rm tetris bin/main.o bin/render.o
