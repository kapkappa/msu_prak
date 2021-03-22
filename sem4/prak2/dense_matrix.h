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
        nonzeros = size-2;

        alloc();

        for (uint64_t i = 0; i < nonzeros; i++) {
            val[i] = *(list.begin()+i+2);
        }
    }

    bool alloc();
    bool generate();
    void print() const;

};
