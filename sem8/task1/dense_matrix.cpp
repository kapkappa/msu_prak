#include "dense_matrix.h"

#include <iomanip>
#include <cstdint>
#include <iostream>
#include <random>
#include <math.h>

void dense_matrix::print() const {
    std::cout << "Matrix shape: " << nrows << " x " << ncols << std::endl;
    for (uint32_t i = 0; i < nrows; i++) {
        std::cout << "row " << i << " : ";
        for (uint32_t j = 0; j < ncols; j++)
            std::cout << std::setw(15) << val[i * nrows + j] << " ";
        std::cout << std::endl;
    }
}

void dense_matrix::generate(uint32_t size) {
    nrows = size;
    ncols = size;
    nonzeros = size * size;
    uint32_t _size = nonzeros - nonzeros % 64 + 64;

    val.alloc(_size);

    for (uint32_t i = 0; i < nrows; i++) {
        for (uint32_t j = 0; j < ncols; j++) {
            val[i * ncols + j] = cos(M_PI + i * j);
        }
    }
}
