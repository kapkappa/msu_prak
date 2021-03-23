#include "matrix.h"
#include "dense_matrix.h"
#include "cassert"

bool dense_matrix::alloc() {
    uint64_t size = nrows * ncols;
    if (size) {
        if_empty = false;
        val.alloc(size);
    }
    return true;
}

bool dense_matrix::generate(const uint32_t &m, const uint32_t &n) {
    if (!if_empty)
        return false;
    nrows = m;
    ncols = n;
    alloc();
    uint64_t size = m * n;
    for (uint64_t i = 0; i < size; i++) {
        val[i] = (double) i;
    }
    nonzeros = size-1;
    return true;
}

double dense_matrix::operator[] (const uint32_t pos) const {
    assert(pos <= nrows * ncols);
    return val[pos];
}

double dense_matrix::get (const uint32_t row, const uint32_t col) const {
    assert(row <= nrows);
    assert(col <= ncols);
    return val[row * nrows + col];
}

dense_matrix operator+ (const dense_matrix & A, const dense_matrix & B) {
    assert(A.nrows == B.nrows);
    assert(A.ncols == B.ncols);
    dense_matrix T(A);
    T.nonzeros = 0;
    uint64_t size = T.nrows * T.ncols;
    for (uint64_t i  = 0; i < size; i++) {
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
    T.nonzeros = 0;
    for (auto i = 0; i < nrows; i++) {
        for (auto j = 0; j < ncols; j++) {
            for (uint32_t k = 0; k < A.ncols; k++) {
                T.val[i * nrows + j] += A.get(i,k) * B.get(k,j);
            }
            if (T.get(i,j) != 0.0)
                T.nonzeros++;
        }
    }
    return T;
}

dense_matrix operator* (const dense_matrix & A, const double coef) {
    double nrows = (double)A.nrows, ncols = (double)A.ncols;
    dense_matrix T(A);
    uint64_t size = nrows * ncols;
    for (uint64_t i = 0; i < size; i++) {
        T.val[i] *= coef;
    }
    if (coef == 0.0)
        T.nonzeros = 0;
    return T;
}

void dense_matrix::print() const {
    if(if_empty)
        return;
    for (uint32_t i = 0; i < nrows; i++) {
        for (uint32_t j = 0; j < ncols; j++)
            std::cout << val[i * ncols + j] << "  ";
        std::cout << std::endl;
    }
}

std::ostream& operator<< (std::ostream& os, const dense_matrix &m) {
    m.print();
    return os;
}
