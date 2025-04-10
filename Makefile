CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lraylib -lm # math.h
BUILD=build
ASSETS_DIR = assets
BUILD_ASSETS_DIR = $(BUILD)/assets

# Find all assets
ASSETS = $(wildcard $(ASSETS_DIR)/*);
BUILD_ASSETS = $(ASSETS:$(ASSETS_DIR)%=$(BUILD_ASSETS_DIR)/%)

all: breakout

breakout: breakout.o $(BUILD)/vector.o
	$(CC) $(BUILD)/breakout.o $(BUILD)/vector.o $(LIBS) $(CFLAGS) -o breakout 

breakout.o: breakout.c
	$(CC) -c breakout.c $(CFLAGS) -o $(BUILD)/breakout.o

$(BUILD)/vector.o: vector.c vector.h build
	$(CC) $(CFLAGS) -c vector.c -o $(BUILD)/vector.o $(LIBS) 

assets: build_assets $(BUILD_ASSETS)

build_assets:
	mkdir -p $(BUILD_ASSETS_DIR)

$(BUILD_ASSETS_DIR)/%: $(ASSETS_DIR)/%
	cp $< $@

build:
	mkdir -p $(BUILD)

clean:
	rm -fr $(BUILD) breakout

run: breakout
	./breakout


.PHONY: clean build build_assets assets all run

