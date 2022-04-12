#pragma once

#include <vector>
#include <cstdint>
#include <stdlib.h>

struct dense_matrix {
public:

    double * val = nullptr;

    bool is_empty = true, is_allocated = false;
    uint32_t nrows = 0, ncols = 0;
    uint32_t nonzeros = 0;

    dense_matrix(uint32_t _nrows, uint32_t _ncols) : nrows(_nrows), ncols(_ncols) {
        nonzeros = nrows * ncols;
    }

    ~dense_matrix() {
        if (is_allocated)
            free(val);
    }

    void print() const;
    void generate();
    void transpose();
    std::vector<double> get_column(uint32_t ) const;
    std::vector<double> get_column(uint32_t, uint32_t ) const;
    std::vector<double> get_minor_column(uint32_t ) const;
};
