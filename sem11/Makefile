CXX = g++
MPICXX = mpicxx
CXXFLAGS = -O3 -w -Wall -Wextra -g -ffast-math -funroll-loops -ftree-vectorize -fopenmp

MACHINE ?= Desktop

ifeq ($(MACHINE), Polus)
CXXFLAGS += -mcpu=power8
else
CXXFLAGS += -m64 -mavx2 -march=core-avx2
endif

all: clean prog omp mpi hyb

prog:
	$(CXX) $(CXXFLAGS) main.cpp field.cpp -o prog

omp:
	$(CXX) $(CXXFLAGS) main_omp.cpp field.cpp -o prog_omp

mpi:
	$(MPICXX) $(CXXFLAGS) main_mpi.cpp field.cpp -o prog_mpi

hyb:
	$(MPICXX) $(CXXFLAGS) main_hyb.cpp field.cpp -o prog_hyb

clean::
	rm -f prog* a.out
