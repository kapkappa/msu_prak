#include "sparse_matrix.h"
//#include "operations.h"
#include "omp.h"
#include <cassert>
#include <sys/time.h>
#include <iostream>
#include <cmath>

#include <vector>
#include <cstdint>

inline double get_norm(const std::vector<double>& x) {
    double res = 0.0;
    for (const auto& it : x)
        res += it * it;
    return std::sqrt(res);
}

inline double get_error_norm(std::vector<double> x) {
    for (auto& it : x)
        it -= 1;
    return get_norm(x);
}

inline void set_const(std::vector<double>& x, double value) {
    auto nthreads = omp_get_max_threads();
    omp_set_num_threads(nthreads);
    std::cout << __func__ << " nthreads: " << nthreads << std::endl;
    std::cout << "x size: " << x.size() << std::endl;
#pragma omp for
    for (uint32_t i = 0; i < x.size(); i++)
        x[i] = value;
}

inline void spmv(const sparse_matrix& A, const std::vector<double>& x, std::vector<double>& y) {
    assert(y.size() == A.nrows);
    assert(A.ncols == x.size());

    auto nthreads = omp_get_max_threads();
    omp_set_num_threads(nthreads);
    std::cout << __func__ << " nthreads: " << nthreads << std::endl;

    uint32_t nrows = A.nrows;
    uint32_t ncols = A.ncols;
    uint32_t width = A.row_size;

#pragma omp for
    for (uint32_t i = 0; i < nrows; i++) {
        y[i] = 0.0;
        for (uint32_t j = 0; j < A.row_size; j++)
            y[i] += x[A.col[i * width + j]] * A.val[i * width + j];
    }
}

inline double dot(const std::vector<double>& x, const std::vector<double>& y) {
    assert(x.size() == y.size());

    auto nthreads = omp_get_max_threads();
    omp_set_num_threads(nthreads);
    std::cout << __func__ << " nthreads: " << nthreads << std::endl;
    std::cout << "x size: " << x.size() << std::endl;

    double res = 0.0;

#pragma omp for
    for (uint32_t i = 0; i < x.size(); i++)
        res += x[i] * y[i];

    return res;
}


inline void axpby(double alpha, const std::vector<double>& x, double betta, std::vector<double>& y) {
    assert(x.size() == y.size());

    auto nthreads = omp_get_max_threads();
    omp_set_num_threads(nthreads);
    std::cout << __func__ << " nthreads: " << nthreads << std::endl;
    std::cout << "x size: " << x.size() << std::endl;

#pragma omp for
    for (uint32_t i = 0; i < y.size(); i++)
        y[i] = alpha * x[i] + betta * y[i];
}

inline void precond(std::vector<double>& z, const std::vector<double>& diag, const std::vector<double>& r) {
    assert(diag.size() == r.size());

    auto nthreads = omp_get_max_threads();
    omp_set_num_threads(nthreads);
    std::cout << __func__ << " nthreads: " << nthreads << std::endl;

    double point = 0.0;

#pragma parallel for reduction(+:point)
    for (uint32_t j = 0; j < r.size(); j++)
        point += r[j] * diag[j];

    set_const(z, point);
}

inline void print(const std::vector<double>& x) {
    for (const auto& it : x)
        std::cout << it << " ";
    std::cout << std::endl;
}

inline std::vector<double> generate_vector(const sparse_matrix& A) {
    std::vector<double> b(A.nrows, 0.0);

    for (uint32_t i = 0; i < A.nrows; i++) {
        double sum = 0.0;
        for (uint32_t j = 0; j < A.row_size; j++)
            sum += A.val[i * A.row_size + j];
        b[i] = sum;
    }

    return b;
}

//  ||Ax-b||
double get_discrepancy(const sparse_matrix& A, const std::vector<double>& x, const std::vector<double>& b) {
    std::vector<double> difference(x.size(), 0.0);

    spmv(A, x, difference);

    for (uint32_t i = 0; i < difference.size(); i++)
        difference[i] -= b[i];

    return get_norm(difference);
}

bool check_symmetry(const sparse_matrix& A) {
    assert(A.nrows == A.ncols);
    bool check = true;
    uint32_t nrows = A.nrows;
    uint32_t ncols = A.ncols;
    uint32_t row_size = A.row_size;
    for (uint32_t i = 0; i < nrows; i++) {
        for (uint32_t j = 0; j < row_size && A.val[i * row_size + j] != 0; j++) {
            uint32_t col_j = A.col[i * row_size + j];
            uint32_t row_j = row_size + 1;
            for (uint32_t k = 0; k < row_size; k++) {
                if (A.col[col_j * row_size + k] == i) {
                    row_j = k;
                    break;
                }
            }
            if (row_j == (row_size + 1)) {
                std::cout << "No such column " << std::endl;
                return false;
            }
            check = A.val[i * row_size + j] == A.val[col_j * row_size + row_j];
            if (check == false) {
                std::cout << "FALSE " << std::endl;
                return false;
            }

        }
    }

    return check;
}

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
