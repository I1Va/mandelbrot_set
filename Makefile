CC = g++
CSRS = main.cpp
OUTFILE = main.out
BUILD_DIR = build


.PHONY: build

all:
	g++ -O2 $(CSRS) -o ./$(BUILD_DIR)/$(OUTFILE)
	./$(BUILD_DIR)/$(OUTFILE)
