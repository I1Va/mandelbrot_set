# ifeq ($(origin CC),default) # "CC" Variable has makefile default value. If Makefile launch ocurred without "CC" flag, "CC = c" would be defaultly set.
# # So we need set value singly
# 	CC = g++
# endif

# CSRC = main.cpp src/display_funcs.cpp src/args_proc.cpp

# .PHONY: all build

# # #================================================TESTING=======================================================
# TEST_CFLAGS = -DNDEBUG -I./inc -march=native -mtune=native
# TEST_DIR = .
# TEST_BUILD_DIR = tests

# complile_all_versions:
# #$(CC) $(TEST_CFLAGS) -Og $(CSRC) -o $(TEST_DIR)/$(TEST_BUILD_DIR)/Og_mandelbrot_set.exe
# 	$(CC) $(TEST_CFLAGS) -O2 $(CSRC) -o $(TEST_DIR)/$(TEST_BUILD_DIR)/O2_mandelbrot_set.exe
# #$(CC) $(TEST_CFLAGS) -Ofast $(CSRC) -o $(TEST_DIR)/$(TEST_BUILD_DIR)/Ofast_mandelbrot_set.exe

# # -msse4.2 -mavx2
# #  -march=native -mtune=native

# asm_sample:
# 	gcc -S -O2 -msse4.2 -msse4.1 -mavx2 -DNDEBUG godbolt_samp.cpp
# #================================================TESTING=======================================================

CC = g++
OUT_O_DIR = build
OUTFILE_NAME = benchmark.out


CFLAGS = -I./inc -march=native -mtune=native

CSRC := src/args_proc.cpp src/display_funcs.cpp benchmarks.cpp
COBJ = $(addprefix $(OUT_O_DIR)/,$(CSRC:.cpp=.o))

all: $(OUT_O_DIR)/$(OUTFILE_NAME)
	$(OUT_O_DIR)/$(OUTFILE_NAME) -d=1
	@del /S $(OUT_O_DIR)\*.out $(OUT_O_DIR)\*.o

$(OUT_O_DIR)/$(OUTFILE_NAME): $(COBJ)
	$(CC) $^ -o $@


$(COBJ) : $(OUT_O_DIR)/%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
