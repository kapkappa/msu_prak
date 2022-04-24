#pragma once

#include <vector>
#include <cstdint>

struct sparse_matrix {
public:

    std::vector<double> val;
    std::vector<double> col;

    bool is_empty = true;
    uint32_t cube_size = 0;
    uint32_t nrows, ncols = 0;
    uint32_t row_size = 0;
    uint64_t nonzeros = 0;

    sparse_matrix(uint32_t _size) {
        nrows = _size;
        ncols = _size;
    }

    void print() const;
    void generate_cube(uint32_t);
    std::vector<double> get_diag() const;
};
