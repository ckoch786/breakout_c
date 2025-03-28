CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lraylib -lm # math.h
BUILD=build

all: breakout

breakout: breakout.o $(BUILD)/vector.o
	$(CC) $(BUILD)/breakout.o $(BUILD)/vector.o $(LIBS) $(CFLAGS) -o breakout 

breakout.o: breakout.c
	$(CC) -c breakout.c $(CFLAGS) -o $(BUILD)/breakout.o

$(BUILD)/vector.o: vector.c vector.h build
	$(CC) $(CFLAGS) -c vector.c -o $(BUILD)/vector.o $(LIBS) 

build:
	mkdir -p $(BUILD)

clean:
	rm -fr $(BUILD) breakout

run: breakout
	./breakout


.PHONY: clean build all run

