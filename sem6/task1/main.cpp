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
//    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
    return omp_get_wtime();
}

static inline double sgn(double x) {
    if (x > 0)
        return 1.0;
    if (x < 0)
        return -1.0;
    return 0.0;
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

    double * b = (double *)calloc(size, sizeof(double));
    double * hh = (double *)calloc(size, sizeof(double));

    generate_vector(A, b, size);

    std::vector<double> x(size, 0.0);

    A.transpose();

    double t0 = timer();

    for (uint32_t i = 0; i < size-1; i++) {
/////// CREATING HAUSEHOLDER VECTOR hh
        uint32_t len = size - i;

#pragma omp parallel for
        for (uint32_t j = i; j < size; j++)
            hh[j-i] = A.val[i * size + j];

        double hh_norm = get_norm(hh, len);
        hh[0] += sgn(hh[0]) * hh_norm;
        hh_norm = get_norm(hh, len);

#pragma omp parallel for
        for (uint32_t j = 0; j < len; j++)
            hh[j] /= hh_norm;

//////  HAUSEHOLDER DECOMPOSITION
#pragma omp parallel for
        for (uint32_t col = i; col < size; col++) {
            double sum = 0.0;
            for (uint32_t k = 0; k < len; k++)
                sum += 2.0 * hh[k] * A.val[col * size + i + k];
            for (uint32_t k = 0; k < len; k++)
                A.val[col * size + i + k] -= sum * hh[k];
        }

        double sum = 0.0;
        for (uint32_t j = 0; j < len; j++)
            sum += 2.0 * hh[j] * b[j+i];
        for (uint32_t j = 0; j < len; j++)
            b[j+i] -= sum * hh[j];
    }

    double t1 = timer();

    std::vector<double> b_local(size, 0.0);
    for (uint32_t i = 0; i < size; i++)
        b_local[i] = b[i];

    for (int32_t i = size-1; i >= 0; i--) {
        for (uint32_t j = i+1; j < size; j++)
            b[i] -= A.val[j * size + i] * x[j];

        x[i] = b[i] / A.val[i * size + i];
    }

    double t2 = timer();

    A.transpose();

    std::cout << "Hosehold time: " << t1-t0 << std::endl;
    std::cout << "Gauss time: " << t2-t1 << std::endl;
    std::cout << "Total time: " << t2-t0 << std::endl;
    std::cout << "||Ax-b|| = " << get_discrepancy(A, x, b_local) << std::endl;
    std::cout << "Error norm: " << get_error_norm(x) << std::endl;

    return 0;
}
