#include "dense_matrix.h"
#include "operations.h"

#include "omp.h"

#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <cstdint>

int nthreads = 1;

dense_matrix matrix_multiplication(const dense_matrix& A, const dense_matrix& B) {
    assert(A.ncols == B.nrows);
    dense_matrix C(A.nrows, B.ncols);

    for (uint32_t i = 0; i < A.nrows; i++) {
        for (uint32_t k = 0; k < B.nrows; k++) {
            double r = A.val[i * A.nrows + k];
            for (uint32_t j = 0; j < B.ncols; j++)
                C.val[i * C.nrows + j] += r * B.val[k * B.nrows + j];
        }
    }

    C.if_empty = false;
    return C;
}

std::vector<double> matvec_multiplication(const dense_matrix& A, const std::vector<double>& b) {
    assert(A.ncols == b.size());

    uint32_t nrows = A.nrows;
    uint32_t ncols = A.ncols;

    std::vector<double> x;
    x.resize(nrows);

    for (uint32_t i = 0; i < nrows; i++) {
        x[i] = 0;
        for (uint32_t j = 0; j < ncols; j++) {
            x[i] += A.val[i * nrows + j] * b[j];
        }
    }

    return x;
}

double get_norm(const std::vector<double>& x) {
    double res = 0.0;
    for (const auto& it : x)
        res += it * it;
    return std::sqrt(res);
}

double sgn(double x) {
    if (x > 0)
        return 1.0;
    if (x < 0)
        return -1.0;
    return 0.0;
}

std::vector<double> create_householder_vector(const std::vector<double>& a, uint32_t index) {
    uint32_t len = a.size();
    std::vector<double> x;

    double a_norm = get_norm(a);

    std::vector<double> e(len, 0.0);
    e[0] = a_norm;

    for (uint32_t i = 0; i < len; i++)
        x.emplace_back(a[i] + sgn(a[0]) * e[i]);

    double x_norm = get_norm(x);
    for (uint32_t i = 0; i < len; i++)
        x[i] /= x_norm;

    return x;
}

dense_matrix create_reflection_matrix(const std::vector<double>& x, uint32_t index) {
    uint32_t size = x.size();
    uint32_t fullsize = size + index;

    dense_matrix U(fullsize, fullsize);

    for (uint32_t i = 0; i < index; i++)
        U.val[i * fullsize + i] = 1.0;

    for (uint32_t i = index; i < fullsize; i++) {
        for (uint32_t j = index; j < fullsize; j++) {
            U.val[i * fullsize + j] = -2 * x[i-index] * x[j-index];
        }
        U.val[i * fullsize + i] += 1;
    }
    U.if_empty = false;
    return U;
}

void householder_multiplication(dense_matrix& A, std::vector<double>& y, const std::vector<double>& x) {
    assert(A.ncols == A.nrows);
    assert(y.size() == A.ncols);

    uint32_t fullsize = A.ncols;
    uint32_t size = x.size();
    uint32_t shift = fullsize - size;

    nthreads = omp_get_max_threads();
    omp_set_num_threads(nthreads);

#pragma omp parallel for shared(A, x) schedule(static)
    for (uint32_t col = shift; col < fullsize; col++) {
        double sum = 0.0;
        for (uint32_t k = 0; k < size; k++)
            sum += 2.0 * x[k] * A.val[(k+shift) * fullsize + col];
        for (uint32_t k = 0; k < size; k++)
            A.val[(k+shift) * fullsize + col] -= sum * x[k];
    }

    for (uint32_t i = shift; i < fullsize; i++) {
        double sum = 0.0;
        for (uint32_t j = 0; j < size; j++)
            sum += 2.0 * x[j] * y[j+shift];
        for (uint32_t j = 0; j < size; j++)
            y[j+shift] -= sum * x[j];
    }
}

void print(const std::vector<double>& x) {
    for (const auto& it : x)
        std::cout << it << " ";
    std::cout << std::endl;
}

std::vector<double> solve_gauss(const dense_matrix& A, const std::vector<double>& y) {
    assert(A.ncols == y.size());
    assert(A.ncols == A.nrows);
    uint32_t size = y.size();
    std::vector<double> x(size, 0.0);
    for (int32_t i = size-1; i >= 0; i--) {
        double sum = 0.0;
        #pragma omp parallel for shared(A, x) reduction(+:sum)
        for (uint32_t j = i+1; j < size; j++)
            sum += x[j] * A.val[i * size + j];

        x[i] = (y[i] - sum) / A.val[i * size + i];
    }
    return x;
}

std::vector<double> generate_vector(uint32_t size) {
    std::vector<double> x;
    for (uint32_t i = 0; i < size; i++)
        x.push_back(1);
    return x;
}

std::vector<double> generate_vector(const dense_matrix& A, uint32_t size) {
    assert(A.nrows == size);

    std::vector<double> x;
    for (uint32_t i = 0; i < size; i++) {
        double sum = 0.0;
        for (uint32_t j = 0; j < A.ncols; j++)
            sum += A.val[i * size + j];
        x.push_back(sum);
    }
    return x;
}

double get_discrepancy(const dense_matrix& A, const std::vector<double>& x, const std::vector<double>& b) {
    assert(A.ncols == x.size());
    assert(A.nrows == b.size());
    //||Ax-b||

    std::vector<double> difference = matvec_multiplication(A, x);
    for (uint32_t i = 0; i < difference.size(); i++)
        difference[i] -= b[i];

    return get_norm(difference);
}
