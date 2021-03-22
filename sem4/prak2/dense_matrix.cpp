#include "matrix.h"
#include "dense_matrix.h"
#include "cassert"

bool dense_matrix::alloc() {
    if (nonzeros) {
        if_empty = false;
        val.resize(nonzeros);
    }
    return true;
}

bool dense_matrix::generate() {
    assert(0);
    return true;
}

void Ax_y(std::vector<double> &x, std::vector<double> &y) {
    assert(0);
}

void Axpy(std::vector<double> &x, std::vector<double> &y) {
    assert(0);
}

void dense_matrix::print() const {
    if(if_empty)
        return;

    for (uint32_t i = 0; i < nrows; i++) {
        for (uint64_t j = 0; i < ncols; j++)
            std::cout << val[i*nrows + j] << "  ";
        std::cout << std::endl;
    }
}
