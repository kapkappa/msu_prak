CC=g++
CXX=g++
MPICXX=mpicxx
CXXFLAGS=-O3 -Wall -g -std=c++2a
LDFLAGS=-lm -lrt

all: compare gen_RMAT graphs_reference gen_RMAT_mpi gen_random gen_random_mpi sssp

gen_RMAT_serial: gen_RMAT.o
	$(CXX) $^ -o $@ $(LDFLAGS)

gen_random_serial: gen_random.o
	$(CXX) $^ -o $@ $(LDFLAGS)

gen_RMAT_mpi: gen_RMAT_mpi.cpp
	$(MPICXX) $(CXXFLAGS) $^ -o $@ -lrt

gen_random_mpi: gen_random_mpi.cpp
	$(MPICXX) $(CXXFLAGS) $^ -o $@ -lrt

graphs_reference: main.o sssp_serial_reference.o
	$(CXX) $^ -o $@ $(LDFLAGS)

# write your own file
sssp: main_mpi.cpp sssp.cpp
	$(MPICXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

compare: compare.o
	$(CXX) $^ -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf *.o gen_RMAT gen_RMAT_mpi gen_random gen_random_mpi graphs_reference compare sssp

