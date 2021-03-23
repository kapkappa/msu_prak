BUILD ?= debug
#BUILD = release

CXX = g++
CXXFLAGS = -c

ifeq ($(BUILD), debug)
CXXFLAGS += -Wall -O0
endif

ifeq ($(BUILD), release)
CXXFLAGS += -O3 -DNDEBUG
endif

all: clean prog

prog: dense_matrix.o sparse_matrix.o vector.o menu.o
	$(CXX) dense_matrix.o sparse_matrix.o vector.o menu.o -o prog

menu.o:
	$(CXX) $(CXXFLAGS) menu.cpp

dense_matrix.o:
	$(CXX) $(CXXFLAGS) dense_matrix.cpp

sparse_matrix.o:
	$(CXX) $(CXXFLAGS) sparse_matrix.cpp

vector.o:
	$(CXX) $(CXXFLAGS) vector.cpp

clean:
	rm -f menu.o sparse_matrix.o dense_matrix.o vector.o prog
