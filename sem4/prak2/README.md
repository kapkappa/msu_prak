supporting Class Vector
Parameters:
 - size
 - pointers: begin, end
 - if_allocated

Member functions:
 - constructor(default, copy, initializer list)
 - destructor
 - operator=
 - operator[]: access to specified element, with border checking
 - push_back: adds element to the end
 - pop_back: remove the last element

Class Matrix
Parameters:
 - nrows, ncols, nonzeros
 - if_empty

Pure virtual functions:
 - print()
 - alloc()
 - generate()

Successors:
 - dense_matrix, sparse_matrix

Parameters:
 - Vector::val
 - sparse only: std::vector row, col

Member functions:
 - constructor(default, copy, initialized)
 - destructor
 - operators: '=' '[]'; As non-member, friend function: '+' '<<' '*'.
 - get(i,j)
 - transpose for sparse matrices
