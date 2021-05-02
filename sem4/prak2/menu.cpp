#include "dense_matrix.h"
#include "sparse_matrix.h"

using namespace std;

int main() {
// DENSE

    dense_matrix A1;
    A1.generate(3, 3);
    A1.print();

    dense_matrix A2(A1);
    A1 = A1 * A2;
    A1.print();

    A2 = A2 * 10;
    cout << A2 << endl;

    dense_matrix A3 = {3, 3, 1, 2, 3, 4, 5};
    A3.print();
    A3 = A3 + A3;
    A3.print();

    dense_matrix A4 = A2 * A3;
    A4.print();

// SPARSE

    sparse_matrix B1;
    B1.generate(4, 6);
    B1.print();
    sparse_matrix B2(4, 6);
    cout << B2 << endl;

    sparse_matrix B3 = B1 + B2;
    B3.print();
    sparse_matrix B4;
    transpose(B3, B4);
    cout << endl;
    B4.print();
    sparse_matrix B5 = B3 * B4;
    cout << endl << B5 << endl;
    return 0;
}
