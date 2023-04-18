#pragma once

#include "vector.hpp"

#include <cstdint>
#include <stdlib.h>

struct dense_matrix {
public:

    vector::vector<double> val;

    uint32_t nrows;
    uint32_t ncols;
    uint32_t nonzeros;

    dense_matrix() {
        nrows = 0;
        ncols = 0;
        nonzeros = 0;
    }

    void print() const;
    void generate(uint32_t);
};
