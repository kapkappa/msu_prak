CXX = g++
#CXXFLAGS = -std=c++17 - on polus gcc -v 4 only
CXXFLAGS = -std=c++11 -fopenmp
BUILD ?= Debug

ifeq ($(BUILD), Release)
CXXFLAGS += -O3 -DNDEBUG
endif

ifeq ($(BUILD), Debug)
CXXFLAGS += -O0 -w
endif

all: clean prog

prog:
	$(CXX) $(CXXFLAGS) dense_matrix.cpp operations.cpp main.cpp -o prog

main.o:
	$(CXX) $(CXXFLAGS) -c main.cpp

operations.o:
	$(CXX) $(CXXFLAGS) -c operations.cpp

matrices.o:
	$(CXX) $(CXXFLAGS) -c dense_matrix.cpp

clean:
	rm -rf prog a.out *.o
