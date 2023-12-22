MPICXX = mpicxx

CXX = g++
CXXFLAGS = -std=c++17
XLCFLAGS = -std=c++11
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
MPICXX = mpixlC
endif

all: clean bsort

bsort:
	$(MPICXX) $(CXXFLAGS) bsort.cpp -o bsort

clean:
	rm -rf bsort *.brt *.log *.txt
