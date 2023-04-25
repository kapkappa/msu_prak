#include "vector.hpp"
#include "mpi.h"

#include <climits>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <sys/time.h>

static inline double timer() {
    struct timeval tp;
    struct timezone tzp;

    gettimeofday(&tp, &tzp);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}

inline uint32_t xorshift32(uint32_t i) {
    uint32_t state = i + 1;
    state ^= (state << 13);
    state ^= (state >> 17);
    state ^= (state << 5);
    return state;
}

inline int fastrand(int g_seed) {
    g_seed = 1664525 * g_seed + 1013904223;
    return g_seed;
}

inline int pseudo_rand(uint32_t i) {
    return fastrand(xorshift32(i));
}

template <typename T>
T pseudo_rand(const uint64_t i) {
    T val = pseudo_rand(i);
    return 0.5 * (1.0 + val / INT_MAX);
}

void set_rand(vector::vector<double>& x) {
    for (uint32_t i = 0; i < x.get_size(); i++) {
        x[i] = pseudo_rand<double>(i) - 0.5;
    }
}

void fill_with_number(vector::vector<double>& x, double number) {
    for (uint32_t i = 0; i < x.get_size(); i++) {
        x[i] = number;
    }
}

void print_norm(const vector::vector<double>& y) {
    double norm = 0.0;

    for (size_t i = 0; i < y.get_size(); i++)
        norm += y[i] * y[i];

    std::cout.precision(12);
    std::cout << std::scientific;
    std::cout << "|| X || = | " << std::sqrt(norm) << " |" << std::endl;
}


int compare(const void* a, const void* b) {
    const double* x = (double*) a;
	const double* y = (double*) b;

	if (*x > *y)
		return 1;
	else if (*x < *y)
		return -1;

	return 0;
}

void sort(vector::vector<double>& x) {
    double* __restrict__ x_ptr = x.data();
    size_t size = x.get_size();
    for (size_t i = 0; i < size; i++) {
        for (size_t j = i+1; j < size; j++) {
            if (x[i] > x[j]) {
                std::swap(x_ptr[i], x_ptr[j]);
            }
        }
    }
}

void merge(vector::vector<double>& tmp, vector::vector<double>& x, const vector::vector<double>& local_x, size_t x_size) {
    size_t i = 0, j = 0, k = 0;
    size_t local_x_size = local_x.get_size();

    double*__restrict__ x_ptr = x.data();
    double*__restrict__ tmp_ptr = tmp.data();
    const double*__restrict__ local_x_ptr = local_x.data();

    while (i < x_size && j < local_x_size) {
        if (x_ptr[i] < local_x_ptr[j]) {
            tmp_ptr[k++] = x_ptr[i++];
        } else {
            tmp_ptr[k++] = local_x_ptr[j++];
        }
    }
    while (i < x_size)
        tmp_ptr[k++] = x_ptr[i++];
    while (j < local_x_size)
        tmp_ptr[k++] = local_x_ptr[j++];

    for (i = 0; i < local_x_size + x_size; i++)
        x_ptr[i] = tmp_ptr[i];
}

bool check_is_sorted(const vector::vector<double>& x) {
    for (size_t i = 1; i < x.get_size(); i++)
        if (x[i] < x[i-1])
            return false;
    return true;
}



int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int rank, world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status Stat;

    size_t VEC_SIZE = std::atoi(argv[1]);

    vector::vector<double> x(VEC_SIZE);
    vector::vector<double> tmp(VEC_SIZE);

    size_t local_size = VEC_SIZE / world_size;
    size_t current_size = local_size;

    vector::vector<double> local_x(local_size);
    fill_with_number(local_x, 0.0);
    fill_with_number(tmp, 0.0);
    fill_with_number(x, 0.0);

    if (rank == 0) {
        set_rand(x);
        print_norm(x);
    }

    double *x_ptr = x.data();
    double *local_x_ptr = local_x.data();

    MPI_Barrier(MPI_COMM_WORLD);
    double t1 = timer();

    //1. split x between all processes
        MPI_Scatter(x_ptr, local_size, MPI_DOUBLE, local_x_ptr, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //2. each process sort its vector
//        sort(local_x);
        qsort(local_x.data(), local_x.get_size(), sizeof(double), compare);

    //3. merge(?)
        if (rank == 0) {
            for (size_t i = 0; i < local_size; i++) {
                x[i] = local_x[i];
            }
        }

        for (int i = 1; i < world_size; i++) {
            if (rank == i) {
                MPI_Send(local_x_ptr, local_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
            } else if (rank == 0) {
                MPI_Recv(local_x_ptr, local_size, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &Stat);
                merge(tmp, x, local_x, current_size);
                current_size += local_size;
            }
        }


    MPI_Barrier(MPI_COMM_WORLD);
    double t2 = timer();

    if (rank == 0) {
        print_norm(x);
        std::cout << "Is sorted: " << check_is_sorted(x) << std::endl;
        std::cout << "Programm ver.1, performance time: " << t2 - t1 << " sec" << std::endl;
    }

    MPI_Finalize();

    return 0;
}
