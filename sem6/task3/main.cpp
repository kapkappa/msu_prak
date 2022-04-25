#include "sparse_matrix.h"
#include "operations.h"
#include "omp.h"
#include <cassert>
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

int main(int argc, char** argv) {
    auto nthreads = omp_get_max_threads();
    omp_set_num_threads(nthreads);
    std::cout << "Threads number: " << omp_get_max_threads() << std::endl;

    uint32_t cube_size = 10;
    uint32_t max_solver_iters = 50;
    uint32_t nwarmups = 10, run_iters = 50;

    if (argc > 1)
        cube_size = atoi(argv[1]);
    if (argc > 2)
        max_solver_iters = atoi(argv[2]);
    if (argc > 3)
        run_iters = atoi(argv[3]);
    if (argc > 4)
        nwarmups = atoi(argv[4]);

    uint32_t size = cube_size * cube_size * cube_size;

    std::cout << "Run conditions:\n"
        << "\tcube size: " << cube_size << "^3\n"
        << "\tmatrix size: " << size << '\n'
        << "\tmaximum solver iterations: " << max_solver_iters << '\n'
        << "\trun iters: " << run_iters << '\n'
        << "\tnumber of warmup runs: " << nwarmups << std::endl;

    run_iters += nwarmups;

    sparse_matrix A(size);
    double gt_0 = timer();
    A.generate_cube(cube_size);
    double gt_1 = timer();
    std::cout << "Generation time: " << gt_1 - gt_0 << std::endl;
//    A.print();
    assert(check_symmetry(A));

    std::vector<double> b = generate_vector(A);

    std::vector<double> x(size, 0.0);
    std::vector<double> r(size, 0.0);
    std::vector<double> z(size, 0.0);
    std::vector<double> p(size, 0.0);
    std::vector<double> q(size, 0.0);

    double scalar1, scalar2, scalar3;
    double betta, alpha;

    uint32_t k;
    double local_time, all_time = 0.0;

    bool convergence;

    auto m = A.get_diag();

    for (uint32_t iter = 0; iter < run_iters; iter++) {

        k = 1;
        convergence = false;

        double t0 = timer();

        set_const(x, 0.0);
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

            if (get_norm(r) < 1e-10 || ++k >= max_solver_iters)
                convergence = true;
        }

        double t1 = timer();

        local_time = t1 - t0;
        if (iter >= nwarmups)
            all_time += local_time;
    }

    std::cout << "Iters: " << k << std::endl;
    std::cout << "Mean total time: " << all_time / (run_iters - nwarmups) << std::endl;
    std::cout << "Residual: " << get_norm(r) << std::endl;
    std::cout << "||Ax-b|| = " << get_discrepancy(A, x, b) << std::endl;
    std::cout << "Error norm: " << get_error_norm(x) << std::endl;

    return 0;
}
