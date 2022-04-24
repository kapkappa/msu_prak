#include "sparse_matrix.h"
#include "operations.h"

#include "omp.h"

#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <cstdint>

void spmv(const sparse_matrix& A, const std::vector<double>& x, std::vector<double>& y) {
    assert(y.size() == A.nrows);
    assert(A.ncols == x.size());

    uint32_t nrows = A.nrows;
    uint32_t ncols = A.ncols;
    uint32_t width = A.row_size;

    for (uint32_t i = 0; i < nrows; i++) {
        y[i] = 0.0;
        for (uint32_t j = 0; j < A.row_size; j++)
            y[i] += x[A.col[i * width + j]] * A.val[i * width + j];
    }
}

double dot(const std::vector<double>& x, const std::vector<double>& y) {
    assert(x.size() == y.size());
    double res = 0.0;
    for (uint32_t i = 0; i < x.size(); i++)
        res += x[i] * y[i];

    return res;
}


void axpby(double alpha, const std::vector<double>& x, double betta, std::vector<double>& y) {
    assert(x.size() == y.size());

    for (uint32_t i = 0; i < y.size(); i++)
        y[i] = alpha * x[i] + betta * y[i];
}


void print(const std::vector<double>& x) {
    for (const auto& it : x)
        std::cout << it << " ";
    std::cout << std::endl;
}

std::vector<double> generate_vector(const sparse_matrix& A) {
    std::vector<double> b(A.nrows, 0.0);

    for (uint32_t i = 0; i < A.nrows; i++) {
        double sum = 0.0;
        for (uint32_t j = 0; j < A.row_size; j++)
            sum += A.val[i * A.row_size + j];
        b[i] = sum;
    }

    return b;
}

double get_discrepancy(const sparse_matrix& A, const std::vector<double>& x, const std::vector<double>& b) {
    //||Ax-b||

    std::vector<double> difference(x.size(), 0.0);

    spmv(A, x, difference);
    for (uint32_t i = 0; i < difference.size(); i++)
        difference[i] -= b[i];

    return get_norm(difference);
}

double get_error_norm(std::vector<double> x) {
    for (auto& it : x)
        it -= 1;
    return get_norm(x);
}

double get_norm(const std::vector<double>& x) {
    double res = 0.0;
    for (const auto& it : x)
        res += it * it;
    return std::sqrt(res);
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

