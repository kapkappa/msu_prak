CC=g++
CXX=g++
MPICXX=mpicxx
CXXFLAGS=-O3 -Wall -g -std=c++2a
LDFLAGS=-lm -lrt

all: compare gen_random_serial gen_random_mpi gen_RMAT_serial gen_RMAT_mpi sssp_serial sssp_mpi

compare: compare.o
	$(CXX) $^ -o $@

gen_random_serial: gen_random.o graph.o
	$(CXX) $^ -o $@ $(LDFLAGS)

gen_random_mpi: gen_random_mpi.cpp graph.cpp
	$(MPICXX) $(CXXFLAGS) $^ -o $@ -lrt

gen_RMAT_serial: gen_RMAT.o graph.o
	$(CXX) $^ -o $@ $(LDFLAGS)

gen_RMAT_mpi: gen_RMAT_mpi.cpp graph.cpp
	$(MPICXX) $(CXXFLAGS) $^ -o $@ -lrt

sssp_serial: main.o sssp_serial_reference.o graph.o
	$(CXX) $^ -o $@ $(LDFLAGS)

sssp_mpi: main_mpi.cpp sssp.cpp graph.cpp
	$(MPICXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf *.o compare gen_random_serial gen_random_mpi gen_RMAT_serial gen_RMAT_mpi sssp_serial sssp_mpi
