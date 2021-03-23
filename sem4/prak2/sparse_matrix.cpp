#include "matrix.h"
#include "sparse_matrix.h"
#include "cassert"

bool sparse_matrix::alloc() {
    if (nonzeros) {
        if_empty = false;
        val.alloc(nonzeros);
        row.resize(nrows+1);
        col.resize(nonzeros);
    }
    return true;
}

bool sparse_matrix::generate(const uint32_t &m, const uint32_t &n) {
    if (!if_empty)
        return false;
    nrows = m;
    ncols = n;
    nonzeros = std::max(m,n);
    alloc();
    row[0] = 0;
    for (uint64_t i = 0; i < nonzeros; i++) {
        val[i] = (double) i;
        col[i] = i % n;
        row[i+1] = i+1;
    }
    return true;
}

double sparse_matrix::operator[] (const uint32_t pos) const {
    assert(pos <= nrows * ncols);
    return val[pos];
}

double sparse_matrix::get (const uint32_t nrow, const uint32_t ncol) const {
    assert(nrow <= nrows);
    assert(ncol <= ncols);
    for (auto j = row[nrow]; j < row[nrow+1]; j++)
        if (col[j] == ncol)
            return val[j];
    return 0.0;
}

//FIXME!
sparse_matrix operator+ (const sparse_matrix & A, const sparse_matrix & B) {
    assert(0);
    assert(A.nrows == B.nrows);
    assert(A.ncols == B.nrows);
    sparse_matrix T(A);
    T.nonzeros = 0;
    uint64_t size = T.nrows * T.ncols;
    for (uint64_t i  = 0; i < size; i++) {
        T.val[i] += B.val[i];
        if (T.val[i] != 0.0)
            T.nonzeros++;
    }
    return T;
}

//FIXME!
sparse_matrix operator* (const sparse_matrix & A, const sparse_matrix & B) {
    assert(0);
    assert(A.ncols == B.nrows);
    double nrows = (double)A.nrows, ncols = (double)B.ncols;
    sparse_matrix T = {nrows, ncols};
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

sparse_matrix operator* (const sparse_matrix & A, const double coef) {
    sparse_matrix T(A);
    for (uint64_t i = 0; i < T.nonzeros; i++) {
        T.val[i] *= coef;
    }
    if (coef == 0.0)
        T.nonzeros = 0;
    return T;
}

void sparse_matrix::print() const {
    if(if_empty)
        return;
    for (uint32_t i = 0; i < nrows; i++) {
        for (uint32_t j = 0; i < ncols; j++)
            std::cout << val[i*nrows + j] << "  ";
        std::cout << std::endl;
    }
}

std::ostream& operator<< (std::ostream& os, const sparse_matrix &m) {
    m.print();
    return os;
}
