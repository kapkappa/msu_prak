#pragma once

#include <iostream>
#include <cstdint>
#include <cerrno>

struct vector {
public:

    uint32_t size;
    double *start, *end;   // end - points to last element

    bool if_allocated;
    bool if_initialized;
    bool if_empty;

    vector(): size(0), start(nullptr), end(nullptr), if_allocated(false), if_initialized(false), if_empty(true) {}

    vector(const vector &vec) : size(vec.size), if_allocated(vec.if_allocated), if_initialized(vec.if_initialized), if_empty(vec.if_empty) {
        if (vec.if_allocated) {
            size = vec.get_size();
            start = (double*)malloc(sizeof(double) * size);
            memcpy(start, vec.start, size * sizeof(double));
            end = start + size - 1;
        } else {
            start = nullptr;
            end = nullptr;
        }
    }

    void alloc(uint32_t _size) {
        assert(!if_allocated);
        size = _size;
        if (size) {
            start = (double*)calloc(size, sizeof(double));
            end = start + size - 1;
            if_allocated = true;
        }
    }

    ~vector() {
        if (if_allocated) free(start);
    }

    vector& operator= (const Vector& vec) {
        if (if_allocated && vec.if_allocated) {
            assert(size == vec.size);
        }

        if (vec.if_allocated) {
            if (!if_allocated)
                alloc(vec.size);
            memcpy(start, vec.start, size * sizeof(double));
        }

        if_initialized = vec.if_initialized;
        if_empty = vec.if_empty;

        return *this;
    }

    void push_back(double value) {
        size++;
        start = (double*)realloc(start, sizeof(double) * size);
        end = start + size - 1;
        *end = value;

        if (size == 1) {
            if_allocated = true;
            if_initialized = true;
        }
    }

    void pop_back() {
        if (if_empty) return;
        size--;
        start = (double*)realloc(start, size * sizeof(double));
        end = start + size - 1;
        if (size == 0) {
            if_empty = true;
             end++;
        }
    }

    void print() const {
        if (!if_allocated || !if_initialized) return;
        std::cout << "Size: " << size << std::endl;
        for(int i = 0; i < size; i++) {
            std::cout << *(start+i) << " ";
        }
        std::cout << std::endl;
    }

    void clear() {
        if (if_allocated) {
            free(start);
            start = nullptr;
            end = nullptr;
        }
        size = 0;
        if_allocated = false;
        if_initialized = false;
        if_empty = true;
    }
};
