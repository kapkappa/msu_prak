#include "dense_matrix.h"
#include "sparse_matrix.h"

using namespace std;

int main() {
    dense_matrix A;
    sparse_matrix B;
    cout << "here1\n";
    A.generate(3, 3);
    cout << "here2\n";
    A.print();
    cout << "here3\n";
    B.generate(4, 6);
    cout << "here4\n";
    B.print();
    return 0;
}
