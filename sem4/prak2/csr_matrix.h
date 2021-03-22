#include ...

struct csr_matrix : matrix {
    std::vector row;
    std::vector col;
    std::vector val;
