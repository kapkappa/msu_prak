#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <cerrno>
#include <cstdint>
#include <stdlib.h>

namespace vector {

template <typename T>
struct vector {

private:
//public:

    T *ptr;

    size_t size;

    bool if_allocated;

public:

    vector(): ptr(nullptr), size(0), if_allocated(false) {}

    vector(uint64_t _size) : size(_size) {
        alloc(size);
    }

    void alloc(uint64_t);

    ~vector() { if (ptr) free(ptr); }

    const T& operator[] (size_t) const;
    T& operator[] (size_t);

    vector& operator= (const vector&);
    vector& operator= (const std::vector<T>&);

    size_t get_size() const { return size; }

    T* data() { return ptr; }
    const T* data() const { return ptr; }

    void print() const;

    void clear();

    void reverse();
};

template <typename T>
void vector<T>::alloc(uint64_t _size) {
    size = _size;
    auto alloc_size = _size - _size % 64 + 64;
    ptr = nullptr;
    if (size > 0) {
        ptr = (T*)aligned_alloc(64, sizeof(T) * alloc_size);

        if (ptr == nullptr) {
            std::perror("Allocation error");
            exit(1);
        }

        for (size_t i = 0; i < alloc_size; i++)
            *(ptr+i) = 0;

        if_allocated = true;
    }
}


template <typename T>
const T& vector<T>::operator[] (size_t pos) const {
    return *(ptr + pos);
}

template <typename T>
T& vector<T>::operator[] (size_t pos) {
    return *(ptr + pos);
}

template <typename T>
vector<T> &vector<T>::operator= (const vector& vec) {
    if (!vec.if_allocated) return *this;

    size = vec.size;

    alloc(size);

    for(size_t i = 0; i < size; i++)
        *(ptr+i) = vec[i];

    return *this;
}

template <typename T>
vector<T> &vector<T>::operator= (const std::vector<T>& vec) {
    size = vec.size();

    alloc(size);

    for(size_t i = 0; i < size; i++)
        *(ptr+i) = vec[i];

    return *this;
}

template <typename T>
void vector<T>::print() const {
    std::cout << "Size: " << size << std::endl;
    for(size_t i = 0; i < size; i++)
        std::cout << *(ptr + i) << " ";

    std::cout << std::endl;
}

template <typename T>
void vector<T>::clear() {
    if (ptr != nullptr) {
        free(ptr);
        ptr = nullptr;
    }
    size = 0;
    if_allocated = false;
}

template <typename T>
void vector<T>::reverse() {
    for (uint32_t i = 0; i < size / 2; i++)
        std::swap(*(ptr+i), *(ptr+size-i-1));
}

} // namespace vector
