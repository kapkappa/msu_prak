#include <iostream>
#include "string.h"

String::String(const int & a) {
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

int String::s2int() const {
    int res(0), digit;
    bool negative = false;
    if(length) negative = str[0] == '-';
    for(int i = negative; i < length; i++) {
        digit = str[i]-'0';
        if(digit >= 0 && digit < 10) res = res * 10 + digit;
        else {
            std::cout << "String contains non-digit symbols" << std::endl;
            return -1;
        }
    }
    if(negative) res*=-1;
    return res;
}

/*
    float s2float() const {
        float res(0), digit, ppos(0);
        bool negative = false;
        if(length) negative = str[0] == '-';
        for (int i = negative; i < length; i++) {
            if(str[i] == '.' || str[i] == ',') {
                ppos = ++i;
                break;
            }
            digit = str[i]-'0';
            if(digit >= 0 && digit < 10) res = res * 10 + digit;
            else {
                std::cout << "String containts non-digit symbols" << std::endl;
                return -1;
            }
        }
        if(ppos == length) {
            std::cout << "Incorrect float format" << std::endl;
            return -1;
        }
        if(ppos) {
            int tmp = 0;
            for (int i = ppos; i < length; i++) {
                digit = str[i] - '0';
                if(digit >= 0 && digit < 10) tmp = tmp * 10 + digit;
                else {
                    std::cout << "String contains non-digit symbols" << std::endl;
                    return -1;
                }
            }
            std::cout << res << '.' << tmp << '\t' << length-ppos << endl;
            std::cout << tmp / (10 * (length - ppos)) << endl;
        }
        if(negative) res *= -1;
        return res;
    }
*/


String operator+ (const String & cp_str1, const String & cp_str2) {
    String tmp;
    tmp.length = cp_str1.length + cp_str2.length;
    tmp.str = (char*)malloc(sizeof(char) * tmp.length);
    for (int i = 0; i < cp_str1.length; i++)
        tmp.str[i] = cp_str1[i];
    for (int i = 0; i < cp_str2.length; i++)
        tmp.str[cp_str1.length+i] = cp_str2[i];
    return tmp;
}

std::ostream& operator<< (std::ostream& os, const String & cp_str) {
    for(int i = 0; i < cp_str.len(); i++)
        std::cout << cp_str[i];
    return os;
}

std::istream& operator>> (std::istream& is, String & cp_str) {
    char c = 0;
    String tmp;
    c = getchar();
    while (c != '\n') {
        tmp.push_back(c);
        c = getchar();
    }
    if(tmp.len()) cp_str += tmp;
    return is;
}
