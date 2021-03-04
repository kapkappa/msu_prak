#include <iostream>

using namespace std;

class String {

    int length;
    char*str;

public:

    String(): length(0), str(nullptr) {};

    String(const String & cp_str) {
        length = cp_str.length;
        str = (char*)malloc(sizeof(char) * length);
        for (int i = 0; i < length; i++)
            str[i] = cp_str[i];
    }

    String(const int & a) {
        int tmp = a, cp(0), size(0);
        bool neg = false;
        if(tmp < 0) neg = true;
        do {
            size++;
            cp = cp * 10 + abs(tmp % 10);
            tmp /= 10;
        } while(tmp);

        length = size + neg;
        str = (char*)malloc(sizeof(char) * length);

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

    String(const char & a) {
        length = 1;
        str = (char*)malloc(sizeof(char));
        str[0] = a;
    }

    ~String() {
        free(str);
    }

    String operator= (const String & cp_str) {
        length = cp_str.length;
        free(str);
        str = (char*)malloc(sizeof(char) * length);
        for (int i = 0; i < length; i++)
            str[i] = cp_str[i];
        return *this;
    }

    String operator+ (const String & cp_str) {
        String tmp;
        tmp.length = length + cp_str.length;
        tmp.str = (char*)malloc(sizeof(char) * tmp.length);
        for (int i = 0; i < length; i++)
            tmp.str[i] = str[i];
        for (int i = 0; i < cp_str.length; i++)
            tmp.str[length+i] = cp_str[i];
        return tmp;
    }

    String operator+= (const String & cp_str) {
        int tmp = length;
        length += cp_str.length;
        str = (char*)realloc(str, sizeof(char)*length);
        for(int i = 0; i < cp_str.length; i++)
            str[tmp+i] = cp_str[i];
        return *this;
    }

    char operator[] (const int pos) const {
        if(pos>=length || pos < 0) {
            std::cout << "Index is out of borders" << std::endl;
            return 0;
        }
        return str[pos];
    }

    void print() const {
        std::cout << "Length is: " << length << "\t";
        for(int i = 0; i < length; i++) {
            std::cout << str[i];
        }
        std::cout << "\n";
    }
};

int main() {
    cout << "Testing default and char constructor" << endl;
    String a1, a2('a');
    a1.print();
    a2.print();
    a1 = a2;
    a1.print();
    cout << "Testing int constructor" << endl;
    String b1(123), b2(0), b3(-123), b4(-0);
    b1.print();
    b2.print();
    b3.print();
    b4.print();
    cout << "Testing concatenation" << endl;
    String c1 = a1+b1, c2 = a2+b2, c3 = c1+a2+b3, c4 = c1+c2+c3;
    c1.print();
    c2.print();
    c3.print();
    c4.print();
    String c5;
    c5 += b1 + b2 + b3 += b4;
    c5.print();
    return 0;
}

