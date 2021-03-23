#pragma once

#include "matrix.h"
#include "vector.cpp"
#include <vector>
#include <iostream>
#include "cassert"

class sparse_matrix : public matrix {

    Vector val;
    std::vector<uint32_t> col;
    std::vector<uint32_t> row;

public:

    sparse_matrix() {
        nrows = ncols = nonzeros = 0;
        if_empty = true;
    }

    sparse_matrix(const std::initializer_list<double> &list) {
        auto size = list.size();
        if (size == 0)
            std::cout << "Empty list\n";
        else if (size < 2)
            std::cout << "Not enougth params\n";
        else
            std::cout << "Too much params!\n";
        nrows = *list.begin();
        ncols = *(list.begin()+1);

        generate(nrows, ncols);
    }

    sparse_matrix(const sparse_matrix & A) {
        nrows = A.nrows;
        ncols = A.ncols;
        nonzeros = A.nonzeros;
        alloc();
        val = A.val;
        row = A.row;
        col = A.col;
    }

    ~sparse_matrix() {
        val.clear();
        row.clear();
        col.clear();
    }

    sparse_matrix & operator= (const sparse_matrix & A) {
        nrows = A.nrows;
        ncols = A.ncols;
        nonzeros = A.nonzeros;
        val.clear();
        val = A.val;
        if_empty = A.if_empty;
        return *this;
    }

    double operator[] (const uint32_t pos) const;

    friend sparse_matrix operator+ (const sparse_matrix &, const sparse_matrix &);
    friend sparse_matrix operator* (const sparse_matrix &, const sparse_matrix &);
    friend sparse_matrix operator* (const sparse_matrix &, const double);
    friend std::ostream & operator<< (std::ostream &os, const sparse_matrix &);

    double get (const uint32_t, const uint32_t) const;
    bool alloc();
    bool generate (const uint32_t &, const uint32_t &);
    void print() const;
/*
    double get_val (const uint32_t pos) const { return val[pos]; }
    uint32_t get_row (const uint32_t pos) const { return row[pos]; }
    uint32_t get_col (const uint32_t pos) const { return col[pos]; }
*/
};
