#include "matrix.h"
#include "dense_matrix.h"
#include "cassert"

bool dense_matrix::alloc() {
    uint64_t size = nrows * ncols;
    if (size) {
        if_empty = false;
        val.resize(size);
    }
    return true;
}

bool dense_matrix::generate(const uint32_t &m, const uint32_t &n) {
    if (!if_empty)
        return false;
    uint64_t size = m * n;
    val.resize(size);
    for (auto i = 0; i < size; i++) {
        val[i] = (double) i;
    }
    nonzeros = size-1;
    if_empty = false;
    return true;
}

double dense_matrix::operator[] (const int pos) const {
    assert(pos >= 0);
    assert(pos <= nrows * ncols);
    return val[pos];
}

double dense_matrix::get (const int row, const int col) const {
    assert(row >= 0 && row <= nrows);
    assert(col >= 0 && col <= ncols);
    return val[row * nrows + col];
}

dense_matrix operator+ (const dense_matrix & A, const dense_matrix & B) {
    assert(A.nrows == B.nrows);
    assert(A.ncols == B.nrows);
    dense_matrix T(A);
    T.nonzeros = 0;
    uint64_t size = T.nrows * T.ncols;
    for (auto i  = 0; i < size; i++) {
        T.val[i] += B.val[i];
        if (T.val[i] != 0.0)
            T.nonzeros++;
    }
    return T;
}

dense_matrix operator* (const dense_matrix & A, const dense_matrix & B) {
    assert(A.ncols == B.nrows);
    double nrows = (double)A.nrows, ncols = (double)B.ncols;
    dense_matrix T = {nrows, ncols};
    uint64_t size = nrows * ncols;
    for (auto i = 0; i < nrows; i++) {
        for (auto j = 0; j < ncols; j++) {
            for (auto k = 0; k < A.ncols; k++) {
                T.val[i * nrows + j] += A.get(i,k) * B.get(k,j);
            }
            if (T.get(i,j) != 0.0)
                T.nonzeros++;
        }
    }
    return T;
}

void dense_matrix::Ax_y(std::vector<double> &x, std::vector<double> &y) {
    assert(0);
}

void dense_matrix::Axpy(std::vector<double> &x, std::vector<double> &y) {
    assert(0);
}

void dense_matrix::print() const {
    if(if_empty)
        return;

    for (auto i = 0; i < nrows; i++) {
        for (auto j = 0; i < ncols; j++)
            std::cout << val[i*nrows + j] << "  ";
        std::cout << std::endl;
    }
}
