#include "dense_matrix.h"
#include "operations.h"

#include <iostream>

int main(int argc, char** argv) {

    dense_matrix A(10, 10);
    A.generate();
    A.print();

/////////////

//Fill with values
    uint64_t ncols = A.ncols;
    std::vector<double> b;
    for (uint64_t i = 0; i < ncols; i++)
        b.push_back(1);

    std::vector<double> abs_res;

    for (uint64_t i = 0; i < A.ncols-1; i++) {
            std::vector<double> x = create_householder_vector(A.get_minor_column(i), i);
            auto U = create_reflection_matrix(x, i);

            A = matrix_multiplication(U, A);
            std::cout << "A matrix after " << i << " iteration: " << std::endl;
            A.print();

            b = matvec_multiplication(U, b);
    }

//    solve_gauss(A, b, x);

    return 0;
}
