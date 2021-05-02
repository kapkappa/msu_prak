Class Vector
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

Class String
Parameters:
 - length
 - pointer str

Member functions:
 - constructor(default, copy, assignment(std::string, int, char)
 - destructor
 - operators: '=' '+=' '[]' '==' '!=' '<' '<=' '>' '>='; As non-member, friend function: '+' '>>' '<<'.
 - print()
 - len()
 - push_back()
 - pop_back()
 - erase()
 - String_to_integer: s2int(), self-made
 - String_to_float/double/longdouble: s2float/s2double/s2longdouble, NOT self-made, using std::string convertation
