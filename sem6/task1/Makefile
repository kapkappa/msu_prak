CXX = g++
CXXFLAGS = -std=c++17
BUILD ?= Debug

ifeq ($(BUILD), Release)
CXXFLAGS += -O3 -DNDEBUG
endif

ifeq ($(BUILD), Debug)
CXXFLAGS += -O0 -w
endif

all: clean prog

prog: matrices.o operations.o main.o
	$(CXX) dense_matrix.o operations.o main.o -o prog

main.o:
	$(CXX) $(CXXFLAGS) -c main.cpp

operations.o:
	$(CXX) $(CXXFLAGS) -c operations.cpp

matrices.o:
	$(CXX) $(CXXFLAGS) -c dense_matrix.cpp

clean:
	rm -rf prog a.out *.o
