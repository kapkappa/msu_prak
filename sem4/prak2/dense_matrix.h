#include "matrix.h"
#include "vector.cpp"
#include <iostream>
#include "cassert"

struct dense_matrix : matrix {

    Vector val;

    dense_matrix() {
        nrows = ncols = nonzeros = 0;
        if_empty = true;
    }

    dense_matrix(const std::initializer_list<double> &list) {
        auto size = list.size();
        if (size == 0)
            std::cout << "Empty list\n";
        else if (size < 2)
            std::cout << "Not enougth params\n";
        nrows = *list.begin();
        ncols = *(list.begin()+1);
        nonzeros = 0;
        alloc();
        for (uint64_t i = 0; i < size-2; i++) {
            val[i] = *(list.begin()+i+2);
            if (val[i] != 0.0)
                nonzeros++;
        }
        for (uint64_t j = size-2; j < nrows * ncols; j++)
            val[j] = 0.0;
    }

    dense_matrix(const dense_matrix & A) {
        nrows = A.nrows;
        ncols = A.ncols;
        nonzeros = A.nonzeros;
        alloc();
        val = A.val;
    }

    ~dense_matrix() {
        val.clear();
    }

    dense_matrix & operator= (const dense_matrix & A) {
        nrows = A.nrows;
        ncols = A.ncols;
        nonzeros = A.nonzeros;
        val.clear();
//        val.alloc(A.val.get_size());
        val = A.val;
        if_empty = A.if_empty;
        return *this;
    }

    double operator[] (const uint32_t pos) const;

    friend dense_matrix operator+ (const dense_matrix &, const dense_matrix &);
    friend dense_matrix operator* (const dense_matrix &, const dense_matrix &);
    friend dense_matrix operator* (const dense_matrix &, const double);
    friend std::ostream & operator<< (std::ostream &os, const dense_matrix &);

    double get (const uint32_t, const uint32_t) const;
    bool alloc();
    bool generate (const uint32_t &, const uint32_t &);
    void print() const;
};
