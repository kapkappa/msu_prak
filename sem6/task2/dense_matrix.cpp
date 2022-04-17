#include "dense_matrix.h"

#include <iomanip>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

void dense_matrix::print() const {
    std::cout << "Matrix shape: " << nrows << " x " << ncols << std::endl;
    if (!is_allocated || is_empty)
        return;
    for (uint32_t i = 0; i < nrows; i++) {
        std::cout << "row " << i << " : ";
        for (uint32_t j = 0; j < ncols; j++)
            std::cout << std::setw(15) << val[i * nrows + j] << " ";
        std::cout << std::endl;
    }
}

void dense_matrix::generate() {
    std::cout << "Generation started\n";
    std::random_device rd;
    std::mt19937 gen(rd());
//    std::uniform_int_distribution<> dis(-INT_MAX, INT_MAX);
    std::normal_distribution<> dis(0, 1000);
//    std::binomial_distribution<> dis(1000, 0.5);

    val = (double*)malloc(nonzeros * sizeof(double));
    is_allocated = true;

    for (uint32_t i = 0; i < nrows; i++) {
        for (uint32_t j = 0; j < ncols; j++) {
//            val.push_back(dis(gen));
            val[i * nrows + j] = (i+1)/(j+1)+10;
        }
    }
    std::cout << "Generation completed\n";
    is_empty = false;
}

void dense_matrix::transpose() {
    double * res = (double*)malloc(nonzeros * sizeof(double));
    for (uint32_t k = 0; k < nrows * ncols; k++) {
        uint32_t i = k / nrows;
        uint32_t j = k % nrows;
        res[k] = val[j * ncols + i];
    }
    free(val);
    val = res;
}
