CXX = g++
MPICXX = mpicxx
CXXFLAGS = -O3 -w -Wall -Wextra -g -ffast-math -funroll-loops -ftree-vectorize -fopenmp

MACHINE ?= Desktop

ifeq ($(MACHINE), Polus)
CXXFLAGS += -mcpu=power8
else
CXXFLAGS += -m64 -mavx2 -march=core-avx2
endif

all: clean omp mpi

omp:
	$(CXX) $(CXXFLAGS) main.cpp field.cpp -o prog

mpi:
	$(MPICXX) $(CXXFLAGS) main_mpi.cpp field.cpp -o prog_mpi

clean::
	rm -f prog* a.out
