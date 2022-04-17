#pragma once

#include <vector>
#include <stdlib.h>
#include <cstdint>

struct dense_matrix {
public:
    double * val = nullptr;

    bool if_empty = true;
    uint32_t nrows = 0, ncols = 0;
    uint32_t nonzeros = 0;

    dense_matrix(uint32_t _nrows, uint32_t _ncols) : nrows(_nrows), ncols(_ncols) {
        nonzeros = nrows * ncols;
    }

    ~dense_matrix() {
        if (val != nullptr)
            free(val);
    }

    void print() const;
    void generate();
    void transpose();
};
