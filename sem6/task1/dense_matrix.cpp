#include "dense_matrix.h"

#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

void dense_matrix::print() const {
    std::cout << "Matrix shape: " << nrows << " x " << ncols << std::endl;
    if (if_empty)
        return;
    for (uint64_t i = 0; i < nrows; i++) {
        std::cout << "row " << i << " : ";
        for (uint64_t j = 0; j < ncols; j++)
            std::cout << val[i * nrows + j] << " ";
        std::cout << std::endl;
    }
}

void dense_matrix::generate() {
    std::random_device rd;
    std::mt19937 gen(rd());
//    std::uniform_int_distribution<> dis(-INT_MAX, INT_MAX);
//    std::normal_distribution<> dis(0, 1000);
    std::binomial_distribution<> dis(1000, 0.5);
    val.resize(0);
    for (uint64_t i = 0; i < nonzeros; i++)
        val.push_back(dis(gen));
    std::cout << "Generation completed\n";
    if_empty = false;
}
