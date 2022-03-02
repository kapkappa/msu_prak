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

    for (uint64_t i = 0; i < A.nrows; i++) {
        for (uint64_t k = 0; k < B.nrows; k++) {
            double r = A.val[i * A.nrows + k];
            for (uint64_t j = 0; j < B.ncols; j++)
                C.val[i * C.nrows + j] += r * B.val[k * B.nrows + j];
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

void householder_multiplication(dense_matrix& A, std::vector<double>& y, const std::vector<double>& x) {
    assert(A.ncols == A.nrows);
    assert(y.size() == A.ncols);

    auto fullsize = A.ncols;
    auto size = x.size();
    auto shift = fullsize - size;

    std::vector<double> tmp_vec(size, 0.0);

    for (uint64_t col = shift; col < fullsize; col++) {
        for (uint64_t row = shift; row < fullsize; row++) {
            double sum = 0.0;
            for (uint64_t k = shift; k < fullsize; k++)
                if (row != k)
                    sum += -2.0 * x[row-shift] * x[k-shift] * A.val[k * A.ncols + col];
                else
                    sum += (1.0 - 2.0 * x[row-shift] * x[k-shift]) * A.val[k * A.ncols + col];
            tmp_vec[row-shift] = sum;
        }
        for (uint64_t j = shift; j < fullsize; j++)
            A.val[j * A.ncols + col] = tmp_vec[j - shift];
    }

    for (uint64_t i = shift; i < fullsize; i++) {
        double sum = 0;
        for (uint64_t j = shift; j < fullsize; j++) {
            if (i != j)
                sum += -2.0 * x[i-shift] * x[j-shift] * y[j];
            else
                sum += (1.0 - 2.0 * x[i-shift] * x[j-shift]) * y[j];
        }
        tmp_vec[i-shift] = sum;
    }
    for (uint64_t i = shift; i < fullsize; i++)
        y[i] = tmp_vec[i-shift];
}

void print(const std::vector<double>& x) {
    for (const auto& it : x)
        std::cout << it << " ";
    std::cout << std::endl;
}

std::vector<double> solve_gauss(const dense_matrix& A, const std::vector<double>& y) {
    assert(A.ncols == y.size());
    assert(A.ncols == A.nrows);
    uint64_t size = y.size();
    std::vector<double> x(size, 0.0);
    for (int64_t i = size-1; i >= 0; i--) {
        double sum = 0.0;
        for (uint64_t j = i; j < size; j++) {
            sum += x[j] * A.val[i * size + j];
        }
        x[i] = (y[i] - sum) / A.val[i * size + i];
    }
    return x;
}

std::vector<double> generate_vector(uint64_t size) {
    std::vector<double> x;
    for (uint64_t i = 0; i < size; i++)
        x.push_back(1);
    return x;
}

std::vector<double> generate_vector(const dense_matrix& A, uint64_t size) {
    assert(A.nrows == size);

    std::vector<double> x;
    for (uint64_t i = 0; i < size; i++) {
        double sum = 0.0;
        for (uint64_t j = 0; j < A.ncols; j++)
            sum += A.val[i * size + j];
        x.push_back(sum);
    }
    return x;
}
