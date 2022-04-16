#include "dense_matrix.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

void dense_matrix::print() const {
    std::cout << "Matrix shape: " << nrows << " x " << ncols << std::endl;
    if (if_empty)
        return;
    for (uint32_t i = 0; i < nrows; i++) {
        std::cout << "row " << i << " : ";
        for (uint32_t j = 0; j < ncols; j++)
            std::cout << val[i * nrows + j] << " ";
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
    val = (double *)calloc(nonzeros, sizeof(double));
    for (uint32_t i = 0; i < nrows; i++) {
        for (uint32_t j = 0; j < ncols; j++) {
//            val.push_back(dis(gen));
            val[i * nrows + j] = (i+1)/(j+1)+10;
        }
    }
    std::cout << "Generation completed\n";
    if_empty = false;
}

std::vector<double> dense_matrix::get_column(uint32_t index) const {
    assert(index < ncols);
    std::vector<double>column;
    for (uint32_t i = 0; i < nrows; i++)
        column.push_back(val[i * nrows + index]);

    return column;
}

std::vector<double> dense_matrix::get_column(uint32_t col, uint32_t row) const {
    assert(col < ncols);
    assert(row < nrows);
    std::vector<double> column;
    for (uint32_t i = row; i < nrows; i++)
        column.push_back(val[i * nrows + col]);

    return column;
}

std::vector<double> dense_matrix::get_minor_column(uint32_t index) const {
    assert(index < ncols);
    assert(index < nrows);
    std::vector<double> column;
    for (uint32_t i = index; i < nrows; i++)
        column.push_back(val[i * nrows + index]);

    return column;
}
