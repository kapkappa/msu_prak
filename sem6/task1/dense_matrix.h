#pragma once

#include <vector>
#include <cstdint>

struct dense_matrix {
public:

    std::vector<double> val;

    bool if_empty = true;
    uint64_t nrows = 0, ncols = 0;
    uint64_t nonzeros = 0;

//    ~dense_matrix;
//    allocate

    dense_matrix(uint64_t _nrows, uint64_t _ncols) : nrows(_nrows), ncols(_ncols) {
        nonzeros = nrows * ncols;
        val.resize(nonzeros);
    }

    void print() const;
    void generate();
    std::vector<double> get_column(uint64_t ) const;
    std::vector<double> get_column(uint64_t, uint64_t ) const;
    std::vector<double> get_minor_column(uint64_t ) const;
};
