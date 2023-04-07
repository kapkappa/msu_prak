CXX = g++
CXXFLAGS = -std=c++17 -fopenmp
XLCFLAGS = -std=c++11 -qsmp=omp
BUILD ?= Debug
MACHINE ?= Desktop

ifeq ($(BUILD), Release)
CXXFLAGS += -O3 -DNDEBUG -ffast-math -funroll-loops -ftree-vectorize -fno-asm -mavx2 -m64 -march=core-avx2 -g
XLCFLAGS += -O5 -DNDEBUG -funroll-loops -qarch=pwr8 -g
endif

ifeq ($(BUILD), Debug)
CXXFLAGS += -O0 -Wall -Wextra
XLCFLAGS += -O0 -Wall -Wextra
endif

ifeq ($(MACHINE), Polus)
CXXFLAGS = $(XLCFLAGS)
CXX = xlc++
endif

all: clean prog

prog:
	$(CXX) $(CXXFLAGS) dense_matrix.cpp ver1.cpp -o prog1
	$(CXX) $(CXXFLAGS) dense_matrix.cpp ver2.cpp -o prog2

clean:
	rm -rf prog1 prog2
