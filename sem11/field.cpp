#include "field.h"

#include <iomanip>

void Field::print(const std::string& txt) const {
    std::cout << "Field: " << txt << std::endl;
    if (array == nullptr || !initialized) {
        std::cout << "Empty field" << std::endl;
        return;
    }

    std::cout << std::scientific << std::setprecision(3);
    for (unsigned int i = 0; i < x_size; i++) {
        printf("x: %d\n", i);
        for (unsigned int j = 0; j < y_size; j++) {
            printf("y=%d: ", j);
            for (unsigned int k = 0; k < z_size; k++) {
                printf("%+1.12E ", array[i * z_size * y_size + j * z_size + k]);
            }
            std::cout << std::endl;
        }
    }
}




Field& Field::operator= (const Field& T) {
    if (this == &T)
        return *this;

    if (array == nullptr) {
        x_size = T.x_size;
        y_size = T.y_size;
        z_size = T.z_size;
        total_size = T.total_size;
        array = (double*)calloc(total_size, sizeof(double));
        initialized = true;
    }
    assert(total_size == T.total_size);
    for (unsigned int i = 0; i < total_size; i++) {
//        array[i] = T.get_ptr()[i];
        array[i] = T.array[i];
    }

    return *this;
}

double& Field::operator() (int i, int j, int k) {
//    assert(i < x_size && i >= 0);
//    assert(j < y_size && j >= 0);
//    assert(k < z_size && k >= 0);
//    assert(initialized);
    return array[i * z_size * y_size + j * z_size + k];
}

const double& Field::operator() (int i, int j, int k) const {
//    assert(i < x_size && i >= 0);
//    assert(j < y_size && j >= 0);
//    assert(k < z_size && k >= 0);
//    assert(initialized);
    return array[i * z_size * y_size + j * z_size + k];
}


Field operator+ (const Field& A, const Field& B) {
    assert(A.total_size == B.total_size);
    Field R(A);
    for (unsigned int i = 0; i < B.total_size; i++) {
        R.array[i] += B.array[i];
    }
    return R;
}

