#pragma once
#include <cstdint>

struct matrix {
    uint32_t nrows, ncols;
    uint64_t nonzeros;
    bool if_empty;

    virtual ~matrix(){};

    virtual void print() const = 0;

    virtual bool alloc() = 0;
    virtual bool generate(const uint32_t &, const uint32_t &) = 0;

    uint32_t get_nrows() const { return nrows; }
    uint32_t get_ncols() const { return ncols; }
    uint64_t get_nonzeros() const { return nonzeros; }
};
