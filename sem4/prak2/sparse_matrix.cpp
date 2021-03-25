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
    nonzeros = m;
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

sparse_matrix operator+ (const sparse_matrix & A, const sparse_matrix & B) {
    assert(A.nrows == B.nrows);
    assert(A.ncols == B.ncols);

    sparse_matrix T;
    T.nrows = A.nrows;
    T.ncols = A.ncols;
    T.nonzeros = A.nonzeros + B.nonzeros;

    T.row.resize(T.nrows+1);
    T.row[0] = 0;
    for (uint32_t i = 0; i < A.nrows; i++) {
        T.row[i+1] = A.row[i+1] + B.row[i+1] - B.row[i];
        for (uint32_t j = A.row[i]; j < A.row[i+1]; j++)
            for (uint32_t jj = B.row[i]; jj < B.row[i+1]; jj++)
                if (B.col[jj] == A.col[j]) {
                    T.nonzeros--;
                    T.row[i+1]--;
                }
    }

    T.val.alloc(T.nonzeros);
    T.col.resize(T.nonzeros);

    for (uint32_t i = 0; i < T.nrows; i++) {
        for (uint32_t j = A.row[i]; j < A.row[i+1]; j++) {
            T.col[j] = A.col[j];
            T.val[j] = A.val[j];
            for (uint32_t jj = B.row[i]; jj < B.row[i+1]; jj++)
                if (B.col[jj] == A.col[j])
                    T.val[j] += B.val[jj];
        }
    }
    T.if_empty = false;
    return T;
}

void transpose (const sparse_matrix & A, sparse_matrix & At) {
    At.nrows = A.ncols;
    At.ncols = A.nrows;
    At.nonzeros = A.nonzeros;
    At.alloc();

    std::vector<uint32_t> col_;
    std::vector<double> val_;
    std::vector<uint32_t> nelems_per_col(A.ncols, 0);

//1. Fill "elems per col" vector
    for (uint32_t i = 0; i < A.nrows; i++) {
        uint32_t row_size = A.row[i+1] - A.row[i];
        col_.resize(row_size, 0);

        for (uint32_t j = A.row[i]; j < A.row[i+1]; j++)
            col_[j - A.row[i]] = A.col[j];

        for (uint32_t ii = 0; ii < col_.size(); ii++)
            nelems_per_col[col_[ii]]++;
    }
//2. Fill transp row vector
    At.row[0] = 0;
    for (uint32_t i = 0; i < At.nrows; i++) {
        At.row[i+1] = At.row[i] + nelems_per_col[i];
        nelems_per_col[i] = 0;
    }
//3. Fill col: col-number == row_number, and elems col_index == col-number + nonzeros elems before
    for (uint32_t i = 0; i < A.nrows; i++) {
        uint32_t row_size = A.row[i+1] - A.row[i];
        col_.resize(row_size, 0);
        val_.resize(row_size, 0.0);

        for (uint32_t j = A.row[i]; j < A.row[i+1]; j++) {
            col_[j - A.row[i]] = A.col[j];
            val_[j - A.row[i]] = A.val[j];
        }
        for (uint32_t ii = 0; ii < col_.size(); ii++) {
            uint32_t Row_num = col_[ii];
            uint32_t Row_offset = At.row[Row_num];

            At.col[Row_offset + nelems_per_col[Row_num]] = i;
            At.val[Row_offset + nelems_per_col[Row_num]] = val_[ii];

            nelems_per_col[Row_num]++;
        }
    }
}

//FIXME!
sparse_matrix operator* (const sparse_matrix & A, const sparse_matrix & B) {
    assert(0);
    assert(A.ncols == B.nrows);

    sparse_matrix T;
    T.nonzeros = 0;
/*
    for (auto i = 0; i < T.nrows; i++) {
        for (auto j = 0; j < T.ncols; j++) {
            for (uint32_t k = 0; k < A.ncols; k++) {
                T.val[i * nrows + j] += A.get(i,k) * B.get(k,j);
            }
            if (T.get(i,j) != 0.0)
                T.nonzeros++;
        }
    }
*/
    return T;
}

sparse_matrix operator* (const sparse_matrix & A, const double coef) {
    sparse_matrix T(A);
    if (coef == 0.0) {
        for (uint32_t i = 0; i < T.nonzeros; i++) {
            T.val[i] = 0.0;
            T.col[i] = 0;
        }
        for (uint32_t j = 0; j < T.nrows+1; j++)
            T.row[j] = 0;
        T.nonzeros = 0;
    } else
        for (uint64_t i = 0; i < T.nonzeros; i++)
            T.val[i] *= coef;
    return T;
}

void sparse_matrix::print() const { std::cout << *this; }

std::ostream& operator<< (std::ostream& out, const sparse_matrix &m) {
    if(m.if_empty)
        return out;
    for (uint32_t i = 0; i < m.nrows; i++) {
        out << "row: " << i;
        for (uint32_t j = m.row[i]; j < m.row[i+1] ; j++)
            out << "\tval: " << m.val[j] << "  col:  " << m.col[j];
        out << std::endl;
    }
    return out;
}
