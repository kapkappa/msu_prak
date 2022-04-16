#include "dense_matrix.h"
#include "operations.h"

#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <cstdint>

namespace {

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

void print(const std::vector<double>& x) {
    for (const auto& it : x)
        std::cout << it << " ";
    std::cout << std::endl;
}

std::vector<double> generate_vector(uint32_t size) {
    std::vector<double> x;
    for (uint32_t i = 0; i < size; i++)
        x.push_back(1);
    return x;
}

void generate_vector(const dense_matrix& A, double * x, uint32_t size) {
    assert(A.nrows == size);

    for (uint32_t i = 0; i < size; i++) {
        double sum = 0.0;
        for (uint32_t j = 0; j < A.ncols; j++)
            sum += A.val[i * size + j];
        x[i] = sum;
    }
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

double get_manhattan_norm(const std::vector<double>& x) {
    double sum = 0.0;
    for (const auto& it : x)
        sum += it;
    return sum;
}

double get_error_norm(std::vector<double> x) {
    for (auto& it : x)
        it -= 1;
    return get_norm(x);
}
