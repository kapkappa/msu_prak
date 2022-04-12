#include "dense_matrix.h"
#include "operations.h"

#include "omp.h"

#include <sys/time.h>
#include <iostream>
#include <cmath>

static inline double timer() {
    struct timeval tp;
    struct timezone tzp;

    gettimeofday(&tp, &tzp);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}

static inline double sgn(double x) {
    if (x > 0)
        return 1.0;
    if (x < 0)
        return -1.0;
    return 0.0;
}

static inline double get_norm(const std::vector<double>& x, uint32_t len) {
    double res = 0.0;
    for (uint32_t it = 0; it < len; it++)
        res += x[it] * x[it];
    return std::sqrt(res);
}

int nthreads = 1;

int main(int argc, char** argv) {
    nthreads = omp_get_max_threads();
    omp_set_num_threads(nthreads);
    std::cout << "Threads number: " << omp_get_max_threads() << std::endl;

    uint32_t size;
    if (argc == 1)
        size = 10;
    else
        size = atoi(argv[1]);

    dense_matrix A(size, size);
    A.generate();

    std::vector<double> b = generate_vector(A, size);
    std::vector<double> hh(size, 0.0);

    uint32_t nrows = A.nrows, ncols = A.ncols;

    double t0 = timer();

    for (uint32_t i = 0; i < size-1; i++) {
/////// CREATING HAUSEHOLDER VECTOR hh
        uint32_t len = nrows - i;

#pragma omp parallel for shared(A, hh)
        for (uint32_t j = i; j < nrows; j++)
            hh[j-i] = A.val[j * ncols + i];

        double hh_norm = get_norm(hh, len);
        hh[0] += sgn(hh[0]) * hh_norm;
        hh_norm = get_norm(hh, len);

#pragma omp parallel for shared(hh)
        for (uint32_t j = 0; j < len; j++)
            hh[j] /= hh_norm;

//////  HAUSEHOLDER DECOMPOSITION
#pragma omp parallel for shared(A, hh) schedule(static)
        for (uint32_t col = i; col < ncols; col++) {
            double sum = 0.0;
            for (uint32_t k = 0; k < len; k++)
                sum += 2.0 * hh[k] * A.val[(k+i) * ncols + col];
            for (uint32_t k = 0; k < len; k++)
                A.val[(k+i) * ncols + col] -= sum * hh[k];
        }

        double sum = 0.0;
        for (uint32_t j = 0; j < len; j++)
            sum += 2.0 * hh[j] * b[j+i];
        for (uint32_t j = 0; j < len; j++)
            b[j+i] -= sum * hh[j];
    }

    double t1 = timer();

    std::vector<double> x = solve_gauss(A, b);

    double t2 = timer();

    std::cout << "Hosehold time: " << t1-t0 << std::endl;
    std::cout << "Gauss time: " << t2-t1 << std::endl;
    std::cout << "Total time: " << t2-t0 << std::endl;
    std::cout << "Error norm: " << get_error_norm(x) << std::endl;
    std::cout << "||Ax-b|| = " << get_discrepancy(A, x, b) << std::endl;

    return 0;
}
