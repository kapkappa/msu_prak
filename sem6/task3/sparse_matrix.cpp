#include "sparse_matrix.h"

#include <iomanip>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>
#include <math.h>

namespace {

static inline double get_number(uint32_t i, uint32_t j) {
    return cos(i * j + M_PI);
//    return i * j + 10;
}

}

void sparse_matrix::print() const {
    if (is_empty)
        return;

//    std::cout << std::scientific << std::setprecision(3);

    std::cout << "Matrix shape: " << nrows << " x " << ncols << std::endl;
    std::cout << "Maximum nonzeros per rows: " << row_size << std::endl;
    std::cout << "Values: " << std::endl;
    for (uint32_t i = 0; i < nrows; i++) {
        std::cout << "row " << std::setw(3) << i << " : ";
        for (uint32_t j = 0; j < row_size; j++)
            std::cout << std::setw(5) << (uint64_t)col[i * row_size + j] << ":" << std::setw(10) << val[i * row_size + j] << " ";
        std::cout << std::endl;
    }
}

void sparse_matrix::generate_cube(uint32_t _cube_size) {
    std::cout << "Generation started\n";

//    val.resize(nrows * row_size);
//    col.resize(nrows * row_size);

    cube_size = _cube_size;
    row_size = 7;

    for (uint32_t K = 0; K < cube_size; K++) {
        for (uint32_t J = 0; J < cube_size; J++) {
            for (uint32_t I = 0; I < cube_size; I++) {
                uint32_t i = K * cube_size * cube_size + J * cube_size + I;
                uint32_t k;
                uint32_t j = 0;
                uint32_t diag_position = 0;
                if (K > 0) {
                    k = (K - 1) * cube_size * cube_size + J * cube_size + I;
                    val.push_back(get_number(i, k));
                    col.push_back(k);
                    j++;
                    nonzeros++;
                }
                if (J > 0) {
                    k = K * cube_size * cube_size + (J - 1) * cube_size + I;
                    val.push_back(get_number(i, k));
                    col.push_back(k);
                    j++;
                    nonzeros++;
                }
                if (I > 0) {
                    k = K * cube_size * cube_size + J * cube_size + I - 1;
                    val.push_back(get_number(i, k));
                    col.push_back(k);
                    j++;
                    nonzeros++;
                }
                val.push_back(0.0);
                col.push_back( K * cube_size * cube_size + J * cube_size + I );
                diag_position = j;
                j++;
                if (I + 1 != cube_size) {
                    k = K * cube_size * cube_size + J * cube_size + I + 1;
                    val.push_back(get_number(i, k));
                    col.push_back(k);
                    j++;
                    nonzeros++;
                }
                if (J + 1 != cube_size) {
                    k = K * cube_size * cube_size + (J + 1) * cube_size + I;
                    val.push_back(get_number(i, k));
                    col.push_back(k);
                    j++;
                    nonzeros++;
                }
                if (K + 1 != cube_size) {
                    k = (K + 1) * cube_size * cube_size + J * cube_size + I;
                    val.push_back(get_number(i, k));
                    col.push_back(k);
                    j++;
                    nonzeros++;
                }
                for (uint32_t jj = j; jj < row_size; jj++) {
                    val.push_back(0.0);
                    col.push_back(0.0);
                }
                double sum = 0.0;
                for (uint32_t jj = 0; jj < row_size; jj++)
                    sum += abs(val[i * row_size + jj]);
                val[i * row_size + diag_position] = 1.5 * sum;
                nonzeros++;
            }
        }
    }
    is_empty = false;
    std::cout << "Generation completed, nonzeros: " << nonzeros << "\n";
}

std::vector<double> sparse_matrix::get_diag() const {
    std::vector<double> result(nrows, 0.0);

    for (uint32_t i = 0; i < nrows; i++) {
        uint32_t j = 0;
        while (col[i * row_size + j] != i) {
            assert(j < row_size);
            j++;
        }
        result[i] = val[i * row_size + j];
    }

    return result;
}

