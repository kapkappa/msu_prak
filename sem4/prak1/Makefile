BUILD ?= debug
#BUILD = release

CXX = g++
CXXFLAGS = -c

ifeq ($(BUILD), debug)
CXXFLAGS += -Wall -O0
endif

ifeq ($(BUILD), release)
CXXFLAGS += -O3
endif

all: clean prog

prog: string.o menu.o
	$(CXX) string.o menu.o -o prog

menu.o:
	$(CXX) $(CXXFLAGS) menu.cpp

string.o:
	$(CXX) $(CXXFLAGS) string.cpp

clean:
	rm -f menu.o string.o prog
