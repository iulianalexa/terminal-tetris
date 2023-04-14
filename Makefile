CC := gcc
CFLAGS := -Wall -lncurses -g
SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src%,bin%,$(patsubst %.c,%.o,$(SOURCES)))
TARGET := tetris

build: $(TARGET)

tetris: $(OBJECTS) bin/main.o
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) bin/main.o
	
bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	
bin/main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o bin/main.o

run: tetris
	./tetris
	
clean:
	rm tetris bin/*
