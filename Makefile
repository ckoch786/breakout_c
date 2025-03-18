CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lraylib -lm

all: breakout

breakout: breakout.c
	$(CC) breakout.c -o breakout $(LIBS) $(CFLAGS)

run: breakout
	./breakout

clean:
	rm -f breakout
