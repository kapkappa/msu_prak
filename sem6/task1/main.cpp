#include "dense_matrix.h"
#include "operations.h"

#include <iostream>

int main(int argc, char** argv) {

    dense_matrix A(10, 10);
    A.generate();
    A.print();

    uint64_t ncols = A.ncols;
    std::vector<double> b = generate_vector(A, ncols);

    std::vector<double> abs_res;

    for (uint64_t i = 0; i < A.ncols-1; i++) {
            std::vector<double> x = create_householder_vector(A.get_minor_column(i), i);
//            auto U = create_reflection_matrix(x, i);
//            A = matrix_multiplication(U, A);
//            b = matvec_multiplication(U, b);

            householder_multiplication(A, b, x);

            std::cout << "A matrix after " << i << " iteration: " << std::endl;
            A.print();

    }

    std::vector<double> x = solve_gauss(A, b);
    print(x);

    return 0;
}
