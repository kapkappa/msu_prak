CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -g -ffast-math -funroll-loops -ftree-vectorize
#CXXFLAGS = -O0 -Wall -Wextra -g

all: clean prog

prog:
	$(CXX) $(CXXFLAGS) main.cpp field.cpp -o prog

clean::
	rm -f prog
