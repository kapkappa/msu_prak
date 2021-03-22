#include "matrix.h"
#include "dense_matrix.h"
#include "cassert"

bool dense_matrix::alloc() {
    int size = nrows * ncols;
    if (size) {
        if_empty = false;
        val.resize(size);
    }
    return true;
}

bool dense_matrix::generate() {
    assert(0);
    return true;
}

dense_matrix operator+ (const dense_matrix & A, const dense_matrix & B) {
    assert(A.nrows == B.nrows);
    assert(A.ncols == B.nrows);
    dense_matrix T(A);
    T.nonzeros = 0;
    int size = T.nrows * T.ncols;
    for (int i  = 0; i < size; i++) {
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
    assert(0);
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

    for (uint32_t i = 0; i < nrows; i++) {
        for (uint64_t j = 0; i < ncols; j++)
            std::cout << val[i*nrows + j] << "  ";
        std::cout << std::endl;
    }
}
