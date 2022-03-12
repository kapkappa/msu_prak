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

static inline double get_norm(const std::vector<double>& x) {
    double res = 0.0;
    for (const auto& it : x)
        res += it * it;
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
    uint32_t nrows = A.nrows, ncols = A.ncols;

    double t0 = timer();

    for (uint32_t i = 0; i < size-1; i++) {
/////// CREATING HAUSEHOLDER VECTOR X
        uint32_t len = nrows - i;
        std::vector<double> x(len, 0.0);

#pragma omp parallel for shared(A, x)
        for (uint32_t j = i; j < nrows; j++)
            x[j-i] = A.val[j * nrows + i];

        x[0] += sgn(x[0]) * get_norm(x);

        double x_norm = get_norm(x);

#pragma omp parallel for shared(x)
        for (uint32_t j = 0; j < len; j++)
            x[j] /= x_norm;

//////  HAUSEHOLDER DECOMPOSITION
        uint32_t fullsize = ncols;
        uint32_t shift = fullsize - len;

#pragma omp parallel for shared(A, x) schedule(static)
        for (uint32_t col = shift; col < fullsize; col++) {
            double sum = 0.0;
            for (uint32_t k = 0; k < len; k++)
                sum += 2.0 * x[k] * A.val[(k+shift) * fullsize + col];
            for (uint32_t k = 0; k < len; k++)
                A.val[(k+shift) * fullsize + col] -= sum * x[k];
        }

        for (uint32_t k = shift; k < fullsize; k++) {
            double sum = 0.0;
            for (uint32_t j = 0; j < len; j++)
                sum += 2.0 * x[j] * b[j+shift];
            for (uint32_t j = 0; j < len; j++)
                b[j+shift] -= sum * x[j];
        }

    }

    double t1 = timer();

    std::vector<double> x = solve_gauss(A, b);

    double t2 = timer();

    std::cout << "Hosehold time: " << t1-t0 << std::endl;
    std::cout << "Gauss time: " << t2-t1 << std::endl;
    std::cout << "Total time: " << t2-t0 << std::endl;
    std::cout << "||Ax-b|| = " << get_discrepancy(A, x, b) << std::endl;

    return 0;
}
