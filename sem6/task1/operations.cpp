#include "dense_matrix.h"
#include "operations.h"

#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <cstdint>

dense_matrix matrix_multiplication(const dense_matrix& A, const dense_matrix& B) {
    assert(A.ncols == B.nrows);
    dense_matrix C(A.nrows, B.ncols);
/*
    for (uint64_t i = 0; i < nrows; i++) {
        for (uint64_t k = 0; k < nrows; k++) {
            double r = A.val[i * nrows + k];
            for (uint64_t j = 0; j < ncols; j++)
                C.val[i * nrows + j] += r * B.val[k * nrows + j];
        }
    }
*/

    for (uint64_t i = 0; i < A.nrows; i++) {
        for (uint64_t j = 0; j < B.ncols; j++) {
            C.val[i * C.ncols + j] = 0.0;
            for (uint64_t k = 0; k < A.ncols; k++)
                C.val[i * C.ncols + j] += A.val[i * A.ncols + k] * B.val[k * B.ncols + j];
        }
    }

    C.if_empty = false;
    return C;
}

std::vector<double> matvec_multiplication(const dense_matrix& A, const std::vector<double>& b) {
    assert(A.ncols == b.size());

    uint64_t nrows = A.nrows;
    uint64_t ncols = A.ncols;

    std::vector<double> x;
    x.resize(nrows);

    for (uint64_t i = 0; i < nrows; i++) {
        x[i] = 0;
        for (uint64_t j = 0; j < ncols; j++) {
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

std::vector<double> create_householder_vector(const std::vector<double>& a, uint64_t index) {
    uint64_t len = a.size();
    std::vector<double> x;

    std::cout << "a vec: " << std::endl;
    print(a);
    double a_norm = get_norm(a);
    std::cout << "a norm: " << a_norm << std::endl;

    std::vector<double> e(len, 0.0);
    e[0] = a_norm;
    std::cout << "e vec: " << std::endl;
    print(e);

    for (uint64_t i = 0; i < len; i++)
        x.emplace_back(a[i] - sgn(a[0]) * e[i]);

    print(x);

    double x_norm = get_norm(x);
    for (uint64_t i = 0; i < len; i++)
        x[i] /= x_norm;

    return x;
}

dense_matrix create_reflection_matrix(const std::vector<double>& x, uint64_t index) {
    uint64_t size = x.size();
    uint64_t fullsize = size + index;

    dense_matrix U(fullsize, fullsize);

    for (uint64_t i = 0; i < index; i++)
        U.val[i * fullsize + i] = 1.0;

    for (uint64_t i = index; i < fullsize; i++) {
        for (uint64_t j = index; j < fullsize; j++) {
            U.val[i * fullsize + j] = -2 * x[i-index] * x[j-index];
        }
        U.val[i * fullsize + i] += 1;
    }
    U.if_empty = false;
    return U;
}

void print(const std::vector<double>& x) {
    for (const auto& it : x)
        std::cout << it << " ";
    std::cout << std::endl;
}
