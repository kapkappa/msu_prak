#include "matrix.h"
#include <iostream>
#include "cassert"

struct dense_matrix : matrix {
    std::vector<double> val;

    dense_matrix() {
        nrows = ncols = nonzeros = 0;
        if_empty = true;
    }

    dense_matrix(const std::initializer_list<double> &list) {
        int size = list.size();
        if (size == 0) {
            std::cout << "Empty list\n";
        } else if (size < 2) {
            std::cout << "Not enougth params\n";
        }
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

    friend dense_matrix operator+ (const dense_matrix &, const dense_matrix &);
    friend dense_matrix operator* (const dense_matrix &, const dense_matrix &);

    void Ax_y (std::vector<double> &, std::vector<double> &);
    void Axpy (std::vector<double> &, std::vector<double> &);

    bool alloc();
    bool generate();
    void print() const;

};
