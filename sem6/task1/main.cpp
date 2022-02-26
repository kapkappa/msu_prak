#include "dense_matrix.h"
#include "operations.h"

#include <iostream>

int main(int argc, char** argv) {
    std::cout << "Hi" << std::endl;

    dense_matrix A(2, 2);
    A.generate();
    A.print();
    dense_matrix B(2, 4);
    B.generate();
    B.print();
    A = matrix_multiplication(A, B);
    A.print();

/////////////

//Fill with values
    uint64_t ncols = A.ncols;
//    uint64_t nrows = A.nrows;
    std::vector<double> b;
    for (uint64_t i = 0; i < ncols; i++)
        b.push_back(1);

    b = matvec_multiplication(A, b);

    for (uint64_t i = 0; i < b.size(); i++)
        std::cout << b[i] << " ";
    std::cout << std::endl;

    std::vector<double> abs_res;

    std::cout << get_norm(b) << std::endl;

    for (uint64_t i = 0; i < A.ncols; i++) {
//        create_u;
//        A = U * A;
//        b = U * b;
    }

//    solve_gauss(A, b, x);

    return 0;
}
