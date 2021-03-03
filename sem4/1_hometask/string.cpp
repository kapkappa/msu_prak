#include <iostream>

class String {

public:
    int length;
    char* str;

//public:

    String(): length(0), str(nullptr) {};
    String(const int & a) {
        //convert int to string

        //find length
        int tmp = a, cp(0), size(0);
        bool neg = false;
        if(tmp < 0) neg = true;
        do {
            size++;
            cp = cp * 10 + abs(tmp % 10);
            tmp /= 10;
        } while(tmp);

        length = size + neg;
        str = new char[length];

        int i(0);
        if(neg) {
            str[0] = '-';
            i++;
        }

        do {
            char c = '0' + cp % 10;
            str[i] = c;
            cp /= 10;
            i++;
        } while(cp);

    }

    ~String() {
        delete [] str;
    }

    void print() {
        std::cout << "Length is: " << length << "\t";
        for(int i = 0; i < length; i++) {
            std::cout << str[i];
        }
        std::cout << "\n";
    }
};

int main() {
    String a(123), b(0), c(1), d(-123), e(-0);
    a.print();
    b.print();
    c.print();
    d.print();
    e.print();
    return 0;
}
