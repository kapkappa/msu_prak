#pragma once

#include <vector>
#include <cstdint>

struct dense_matrix {
public:

    std::vector<double> val;

    bool if_empty = true;
    uint32_t nrows = 0, ncols = 0;
    uint32_t nonzeros = 0;

//    ~dense_matrix;
//    allocate

    dense_matrix(uint32_t _nrows, uint32_t _ncols) : nrows(_nrows), ncols(_ncols) {
        nonzeros = nrows * ncols;
        val.resize(nonzeros);
    }

    void print() const;
    void generate();
    std::vector<double> get_column(uint32_t ) const;
    std::vector<double> get_column(uint32_t, uint32_t ) const;
    std::vector<double> get_minor_column(uint32_t ) const;
};
