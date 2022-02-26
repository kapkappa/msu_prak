CXX = g++ -Wall -O0

all: clean prog

prog: matrices.o operations.o main.o
	$(CXX) dense_matrix.o operations.o main.o -o prog

main.o:
	$(CXX) -c main.cpp

operations.o:
	$(CXX) -c operations.cpp

matrices.o:
	$(CXX) -c dense_matrix.cpp

clean:
	rm -rf prog a.out *.o
