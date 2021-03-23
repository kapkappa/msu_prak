#ifndef VEC_CPP
#define VEC_CPP

#include <iostream>
#include <cerrno>

class Vector {
    // Privat
    int size;
    double *start, *end;   // end - points to last elem

    bool if_allocated;

public:
    Vector(): size(0), start(nullptr), end(nullptr), if_allocated(false) {}

    Vector(const Vector &vec) {
        size = vec.get_size();
        if(size) {
            start = (double*)malloc(sizeof(double) * size);
            if(start == nullptr) {
                std::perror("Allocation error");
                exit(1);
            }
            for (int i = 0; i < size; i++)
                *(start + i) = vec[i];
            end = start + size - 1;
            if_allocated = true;
        } else {
            start = nullptr;
            end = nullptr;
        }
    }

    Vector(const std::initializer_list<double> &list) {
        size = list.size();
        if(size) {
            start = (double*)malloc(sizeof(double) * size);
            if(start == nullptr) {
                std::perror("Allocation error");
                exit(1);
            }
            for (int i = 0; i < size; i++)
                *(start + i) = *(list.begin() + i);
            end = start + size - 1;
            if_allocated = true;
        } else {
            start = nullptr;
            end = nullptr;
        }
    }

    void alloc(const int elems) {
        size = elems;
        if(size) {
            start = (double*)malloc(sizeof(double) * size);
            if(start == nullptr) {
                std::perror("Allocation error");
                exit(1);
            }
            end = start + size - 1;
            if_allocated = true;
        } else {
            start = nullptr;
            end = nullptr;
        }
    }

    ~Vector() {
        if(start) free(start);
    }

    double& operator[] (const int pos) const {
        if(0 <= pos && pos < size)
            return *(start + pos);
        else {
            std::cerr << "Out of bounds of vector" << " size is: " << size << " pos is: " << pos << std::endl;
            exit(2);
        }
    }

    Vector& operator= (const Vector& vec) {
        if(!vec.if_allocated) return *this;

        size = vec.size;
        start = (double*)realloc(start, sizeof(double) * size);
        if(start == nullptr) {
            perror("Reallocation error");
            exit(1);
        }
        for(int i = 0; i < size; i++)
            *(start+i) = vec[i];
        end = start + size - 1;
        if(!size) end--;
        return *this;
    }

    void push_back(const double value) {
        size++;
        start = (double*)realloc(start, sizeof(double) * size);
        if(start == nullptr) {
            std::perror("Reallocation error");
            exit(1);
        }
        if_allocated = true;
        end = start + size - 1;
        *end = value;
    }

    void pop_back() {
        if(!size) return;
        size--;
        start = (double*)realloc(start, size * sizeof(double));
        end = start + size - 1;
        if(!size) end++;
    }

    int get_size() const { return size; }

    void print() const {
        std::cout << "Size: " << size << std::endl;
        for(int i = 0; i < size; i++) {
            std::cout << *(start+i) << " ";
        }
        std::cout << std::endl;
    }

    void clear() {
        if(start) free(start);
        size = 0;
        if_allocated = false;
    }
};

#endif //VEC_CPP
