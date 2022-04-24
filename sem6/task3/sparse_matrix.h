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
    uint32_t row_size = 7;

    sparse_matrix(uint32_t _cube_size) : cube_size(_cube_size) {
        nrows = cube_size * cube_size * cube_size;
        ncols = nrows;
    }

    void print() const;
    void generate();
    std::vector<double> get_diag() const;
};
