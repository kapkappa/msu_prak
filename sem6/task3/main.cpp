#include "sparse_matrix.h"
#include "operations.h"

#include <cassert>

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

int nthreads = 1;

int main(int argc, char** argv) {
    nthreads = omp_get_max_threads();
    omp_set_num_threads(nthreads);
    std::cout << "Threads number: " << omp_get_max_threads() << std::endl;

    uint32_t cube_size = 10;
    uint32_t max_iters = 50;

    if (argc >= 2) {
        cube_size = atoi(argv[1]);
        if (argc == 3)
            max_iters = atoi(argv[2]);
    }

    sparse_matrix A(cube_size);
    A.generate();
//    A.print();
    assert(check_symmetry(A));

    uint32_t size = A.nrows;

    std::vector<double> b = generate_vector(A);

    std::vector<double> x(size, 0.0);
    std::vector<double> r(size, 0.0);
    std::vector<double> z(size, 0.0);
    std::vector<double> p(size, 0.0);
    std::vector<double> q(size, 0.0);

    double scalar1, scalar2, scalar3;
    double betta, alpha;

    uint32_t k = 1;

    bool convergence = false;

    auto m = A.get_diag();

    double t0 = timer();

    r = b;
    precond(z, m, r);
    p = z;

    while (!convergence) {
//        std::cout << "Iter: " << k << "   Norm: " << get_norm(r) << std::endl;

        spmv(A, p, q);
        scalar1 = dot(r, z);
        scalar2 = dot(p, q);
        alpha = scalar1 / scalar2;

        axpby(alpha, p, 1.0, x);
        axpby(-alpha, q, 1.0, r);

        precond(z, m, r);

        scalar3 = dot(r, z);
        betta = scalar3 / scalar1;
        axpby(1.0, z, betta, p);

        if (get_norm(r) < 1e-10 || ++k >= max_iters)
            convergence = true;
    }

    double t1 = timer();

    std::cout << "Iters: " << k << " Residual: " << get_norm(r) << std::endl;

//    print(x);

    std::cout << "Total time: " << t1-t0 << std::endl;
    std::cout << "||Ax-b|| = " << get_discrepancy(A, x, b) << std::endl;
    std::cout << "Error norm: " << get_error_norm(x) << std::endl;

    return 0;
}
