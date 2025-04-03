ifeq ($(origin CC),default) # "CC" Variable has makefile default value. If Makefile launch ocurred without "CC" flag, "CC = c" would be defaultly set.
# So we need set value singly
	CC = g++
endif

CSRC = main.cpp src/display_funcs.cpp src/args_proc.cpp

.PHONY: all build

# #================================================TESTING=======================================================
TEST_CFLAGS = -msse4.2 -mavx2 -DNDEBUG -I./inc
TEST_DIR = .
TEST_BUILD_DIR = .

complile_all_versions:
	$(CC) $(TEST_CFLAGS) -Og $(CSRC) -o $(TEST_DIR)/$(TEST_BUILD_DIR)/Og_mandelbrot_set.exe
	$(CC) $(TEST_CFLAGS) -O2 $(CSRC) -o $(TEST_DIR)/$(TEST_BUILD_DIR)/O2_mandelbrot_set.exe
	$(CC) $(TEST_CFLAGS) -Ofast $(CSRC) -o $(TEST_DIR)/$(TEST_BUILD_DIR)/Ofast_mandelbrot_set.exe

asm_sample:
	gcc -S -O2 -msse4.2 -msse4.1 -mavx2 -DNDEBUG godbolt_samp.cpp
#================================================TESTING=======================================================
