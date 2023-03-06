#include "vector.hpp"
#include "dense_matrix.h"
#include "omp.h"
#include <climits>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sys/time.h>

int nthreads = 1;

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

void set_rand(vector::vector<double>& x, uint16_t nv) {
    for (uint32_t i = 0; i < x.get_size(); i += nv) {
        for (uint16_t k = 0; k < nv; k++) {
            x[i + k] = pseudo_rand<double>(i + k) - 0.5;
        }
    }
}

void fill_with_number(vector::vector<double>& x, double number) {
    for (uint32_t i = 0; i < x.get_size(); i++) {
        x[i] = number;
    }
}

void print_norm(vector::vector<double>& y, uint16_t nv) {
    double norm[nv];
    for (size_t i = 0; i < nv; i++)
        norm[i] = 0.0;

    for (size_t i = 0; i < y.get_size(); i += nv)
        for (auto j = 0; j < nv; j++)
            norm[j] += y[i + j] * y[i + j];

    std::cout.precision(12);
    std::cout << std::scientific;
    std::cout << "|| y = A * x || = |";
    for (uint16_t i = 0; i < nv; i++)
        std::cout << " " << std::sqrt(norm[i]) << " |";
    std::cout << std::endl;
}


int main(int argc, char** argv) {
    nthreads = omp_get_max_threads();
    omp_set_num_threads(nthreads);
    std::cout << "nthreads: " <<  nthreads << std::endl;
    uint32_t size = 100, niters = 100;
    uint16_t nv = 1;

    if (argc != 4) {
        std::cout << "matrix size\tniters\tnumber of rhs" << std::endl;
        return 1;
    } else {
        size = std::atoi(argv[1]);
        niters = std::atoi(argv[2]);
        nv = std::atoi(argv[3]);
    }

    dense_matrix matrix;
    matrix.generate(size);

    vector::vector<double> x(size * nv);
    vector::vector<double> y(size * nv);

    set_rand(x, nv);
    set_rand(y, nv);
//    fill_with_number(x, 1.0);

    const double *__restrict__ val_ptr = matrix.val.data();
    const double *__restrict__ x_ptr = x.data();
    double *__restrict__ y_ptr = y.data();

    unsigned int it, i, j;

    double t1 = timer();

    for (it = 0; it < niters; it++) {
//        set_rand(y, nv);
//        fill_with_number(y, 0.0);
#pragma omp parallel for shared(val_ptr, x_ptr, y_ptr, size) private(i, j) schedule(static) collapse(2) nowait
        for (i = 0; i < size; i++) {
#pragma GCC ivdep
            for (j = 0; j < size; j++) {
                y_ptr[i] += val_ptr[i * size + j] * x_ptr[j];
            }
        }
    }

    double t2 = timer();
    std::cout << niters << " MATVEC iters, time: " << t2 - t1 << " sec" << std::endl;
    print_norm(y, nv);

    return 0;
}
