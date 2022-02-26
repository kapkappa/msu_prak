#include "dense_matrix.h"
#include "operations.h"

#include <cmath>
#include <cassert>
#include <vector>
#include <cstdint>

dense_matrix matrix_multiplication(const dense_matrix& A, const dense_matrix& B) {
    assert(A.ncols == B.nrows);
    uint64_t nrows = A.nrows;
    uint64_t ncols = B.ncols;
    dense_matrix C(nrows, ncols);

    for (uint64_t i = 0; i < nrows; i++) {
        for (uint64_t k = 0; k < nrows; k++) {
            double r = A.val[i * nrows + k];
            for (uint64_t j = 0; j < ncols; j++)
                C.val[i * nrows + j] += r * B.val[k * nrows + j];
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

uint64_t get_norm(const std::vector<double>& x) {
    uint64_t res = 0;
    for (const auto it : x)
        res += it * it;
    return std::sqrt(res);
}
