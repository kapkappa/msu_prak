CXX = g++
CXXFLAGS = -std=c++11

BUILD ?= Debug

ifeq ($(BUILD), Release)
CXXFLAGS += -O3 -DNDEBUG -ffast-math -funroll-loops -ftree-vectorize -fno-asm -mavx2 -m64 -march=core-avx2
endif

ifeq ($(BUILD), Debug)
CXXFLAGS += -O0 -w
endif

all: clean prog

prog:
	$(CXX) $(CXXFLAGS) main.cpp -o prog

clean:
	rm -f prog
