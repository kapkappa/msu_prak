CC=g++
CXX=g++
MPICXX=mpicxx
CXXFLAGS=-O3 -Wall -g -std=c++2a #-Wextra
LDFLAGS=-lm -lrt

BUILD ?= Release
MACHINE ?= Desktop

ifeq ($(BUILD), Debug)
CXXFLAGS += -DDEBUG
endif

ifeq ($(MACHINE), Polus)
MPICXX=g++ -I/polusfs/OpenMPI/RHEL-7.4/2.1.3/include -Wl,-rpath -Wl,/polusfs/OpenMPI/RHEL-7.4/2.1.3/lib -Wl,--enable-new-dtags -L/polusfs/OpenMPI/RHEL-7.4/2.1.3/lib -lmpi
endif

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

sssp_serial: main.o sssp.o graph.o
	$(CXX) $^ -o $@ $(LDFLAGS)

sssp_mpi: main_mpi.cpp sssp_mpi.cpp graph.cpp
	$(MPICXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf *.o compare gen_random_serial gen_random_mpi gen_RMAT_serial gen_RMAT_mpi sssp_serial sssp_mpi
