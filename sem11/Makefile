CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -g -ffast-math -funroll-loops -ftree-vectorize -fopenmp

MACHINE ?= Desktop

ifeq ($(MACHINE), Polus)
CXXFLAGS += -mcpu=power8
else
CXXFLAGS += -m64 -mavx2 -march=core-avx2
endif

all: clean prog

prog:
	$(CXX) $(CXXFLAGS) main.cpp field.cpp -o prog

clean::
	rm -f prog
