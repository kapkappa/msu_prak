#include "dense_matrix.h"
#include "operations.h"

#include "omp.h"

#include <sys/time.h>
#include <iostream>

static inline double timer() {
    struct timeval tp;
    struct timezone tzp;

    gettimeofday(&tp, &tzp);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}

int main(int argc, char** argv) {

    std::cout << "Threads number: " << omp_get_max_threads() << std::endl;

    uint64_t size;
    if (argc == 1)
        size = 10;
    else
        size = atoi(argv[1]);

    dense_matrix A(size, size);
    A.generate();

//    A.print();

    std::vector<double> b = generate_vector(A, size);

    double t0 = timer();

    for (uint64_t i = 0; i < size-1; i++) {
        std::vector<double> x = create_householder_vector(A.get_minor_column(i), i);
        householder_multiplication(A, b, x);

//        auto U = create_reflection_matrix(x, i);
//        A = matrix_multiplication(U, A);
//        b = matvec_multiplication(U, b);

//        A.print();
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
