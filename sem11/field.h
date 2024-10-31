#pragma once

#include <string>
#include <iostream>
#include <cassert>
#include <cstdlib>

class Field {
private:
    unsigned int x_size, y_size, z_size, total_size;
//    double * array;
    bool initialized;

public:

    double * array;

    Field() {
        x_size = 0;
        y_size = 0;
        z_size = 0;
        total_size = 0;
        array = nullptr;
        initialized = false;
    }

    Field(int _x_size, int _y_size, int _z_size) : x_size(_x_size), y_size(_y_size), z_size(_z_size) {
        total_size = x_size * y_size * z_size;
        array = (double*)calloc(total_size, sizeof(double));
        initialized = true;
    }

    Field(const Field& T) {
        assert(T.initialized);
        x_size = T.x_size;
        y_size = T.y_size;
        z_size = T.z_size;
        total_size = T.total_size;
        array = (double*)calloc(total_size, sizeof(double));
        for (unsigned int i = 0; i < total_size; i++) {
//            array[i] = T.get_ptr()[i];
            array[i] = T.array[i];
        }
        initialized = true;
    }

    ~Field() {
        if (array != nullptr)
            free(array);
    }

    void init(int _x_size, int _y_size, int _z_size) {
        assert(!initialized);
        x_size = _x_size;
        y_size = _y_size;
        z_size = _z_size;
        total_size = x_size * y_size * z_size;
        array = (double*)calloc(total_size, sizeof(double));
        initialized = true;
    }

    Field& operator= (const Field&);

    double& operator()(int i, int j, int k);
    const double& operator() (int i, int j, int k) const;

    friend Field operator+ (const Field&, const Field&);

    unsigned int get_x_size() const { return x_size; }
    unsigned int get_y_size() const { return y_size; }
    unsigned int get_z_size() const { return z_size; }
    unsigned int get_total_size() const { return total_size; }
    bool is_initialized() const { return initialized; }
//    double * get_ptr() { return array; }
//    double * get_ptr() const { return array; }
    void print(const std::string&) const;
};

