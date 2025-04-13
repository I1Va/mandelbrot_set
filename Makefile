ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

MODE ?= RELEASE

WINDOWS=1

ifeq ($(WINDOWS),1)
	LAUNCH_MKDIR_COM = if not exist "$(subst /,\,$(@D))" mkdir "$(subst /,\,$(@D))"
else
	LAUNCH_MKDIR_COM = mkdir -p $(@D)
endif

ifeq ($(MODE),DEBUG)
	CFLAGS = $(CDEBFLAGS)
	OUT_O_DIR = debug
	EXTRA_FLAGS = $(SANITIZER_FLAGS)
endif

ifeq ($(origin CC),default)
	CC = gсс
endif

ifeq ($(origin CXX),default)
	CXX = g++
endif

CFLAGS ?= -O2 -march=native -mtune=native -DNDEBUG

CDEBFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations \
-Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported \
-Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
-Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual\
-Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override\
-Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros\
-Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs \
-Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging\
-fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla

SANITIZER_FLAGS = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,$\
integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,$\
shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

EXTRA_FLAGS =
LDFLAGS =
OUT_O_DIR ?= build
COMMONINC = -I./inc
SRC = ./src

CSRC = main.cpp src/args_proc.cpp src/control_funcs.cpp $(wildcard src/calc_funcs/*.cpp)

COBJ := $(addprefix $(OUT_O_DIR)/,$(CSRC:.cpp=.o))

DEPS = $(COBJ:.o=.d)
override CFLAGS += $(COMMONINC)


.PHONY: all

OUTFILE_NAME = mandelbrot_set.out

all: $(OUT_O_DIR)/$(OUTFILE_NAME)

$(OUT_O_DIR)/$(OUTFILE_NAME): $(COBJ)
	@$(CXX) $^ -o $@ $(LDFLAGS) $(EXTRA_FLAGS)

$(COBJ) : $(OUT_O_DIR)/%.o : %.cpp
	@$(LAUNCH_MKDIR_COM)
	@$(CXX) $(CFLAGS) -c $< -o $@

$(DEPS) : $(OUT_O_DIR)/%.d : %.cpp
	@$(LAUNCH_MKDIR_COM)
	@$(CXX) -E $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@


launch:
	$(OUT_O_DIR)/$(OUTFILE_NAME) -O=array

.PHONY: clean
clean:
	@rmdir /Q /S $(OUT_O_DIR)

NODEPS = clean


ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
include $(DEPS)
endif
