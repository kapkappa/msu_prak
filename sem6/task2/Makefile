CXX = mpicxx
CXXFLAGS = -std=c++17 -fopenmp
XLCFLAGS = -std=c++11 -qsmp=omp
BUILD ?= Debug
MACHINE ?= Desktop

ifeq ($(BUILD), Release)
CXXFLAGS += -O3 -DNDEBUG -ffast-math -funroll-loops -ftree-vectorize
XLCFLAGS += -O5 -DNDEBUG
endif

ifeq ($(BUILD), Debug)
CXXFLAGS += -O0 -w
XLCFLAGS += -O0 -w
endif

ifeq ($(MACHINE), Polus)
CXXFLAGS = $(XLCFLAGS)
CXX = mpixlC
endif

all: clean prog

prog:
	$(CXX) $(CXXFLAGS) dense_matrix.cpp operations.cpp main.cpp -o prog

clean:
	rm -rf prog
