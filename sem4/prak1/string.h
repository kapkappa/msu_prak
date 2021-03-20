#pragma once

#include <iostream>
#include "string.h"

class String {

private:
    int length;
    char*str;

public:

    friend String operator+ (const String &, const String &);
    friend std::ostream& operator<< (std::ostream &, const String &);
    friend std::istream& operator>> (std::istream &, String &);

    String(): length(0), str(nullptr) {};

    String(const String & cp_str) {
        length = cp_str.length;
        str = (char*)malloc(sizeof(char) * length);
        for (int i = 0; i < length; i++)
            str[i] = cp_str[i];
    }

    String(const std::string & s_str) {
        length = s_str.length();
        str = (char*)malloc(sizeof(char) * length);
        for (int i = 0; i < length; i++)
            str[i] = s_str[i];
    }

    String(const int & a);

    String(const char & a) {
        length = 1;
        str = (char*)malloc(sizeof(char));
        str[0] = a;
    }

    ~String() { if(str) free(str); }

    const String & operator= (const String & cp_str) {
        length = cp_str.length;
        free(str);
        str = (char*)malloc(sizeof(char) * length);
        for (int i = 0; i < length; i++)
            str[i] = cp_str[i];
        return *this;
    }

    const String & operator+= (const String & cp_str) {
        int tmp = length;
        length += cp_str.length;
        str = (char*)realloc(str, sizeof(char)*length);
        for(int i = 0; i < cp_str.length; i++)
            str[tmp+i] = cp_str[i];
        return *this;
    }

    char& operator[] (const int pos) const {
        if(pos>=length || pos < 0) {
            std::cout << "Index is out of borders" << std::endl;
            return str[0];
        }
        return str[pos];
    }

    //lexico-graphic comparison
    bool operator== (const String & cp_str) const {
        if(length != cp_str.length) return false;
        for (int i = 0; i < length; i++)
            if(str[i] != cp_str[i]) return false;
        return true;
    }

    bool operator!= (const String & cp_str) const {
        return !(*this==cp_str);
    }

    bool operator< (const String & cp_str) const {
        int i = 0;
        while(i < length && i < cp_str.length) {
            if (str[i] < cp_str[i]) return true;
            else if(str[i] > cp_str[i]) return false;
            else i++;
        }
        return (length < cp_str.length);
    }

    bool operator<= (const String & cp_str) const {
        return (*this < cp_str || *this == cp_str);
    }

    bool operator>= (const String & cp_str) const {
        return !(*this < cp_str);
    }

    bool operator> (const String & cp_str) const {
        return !(*this <= cp_str);
    }

    int s2int() const;

    float s2float() const {
        std::string s = str;
        return std::stof(s);
    }

    double s2double() const {
        std::string s = str;
        return std::stod(s);
    }

    long double s2longdouble() const {
        std::string s = str;
        return std::stold(s);
    }

    void print() const {
//        std::cout << "Length is: " << length << "\t";
        for(int i = 0; i < length; i++) {
            std::cout << str[i];
        }
        std::cout << "\n";
    }

    void push_back (const char & c) {
        length++;
        str = (char*)realloc(str, sizeof(char) * length);
        str[length-1] = c;
    }

    void pop_back () {
        length--;
        str = (char*)realloc(str, sizeof(char) * length);
    }

    void erase() {
        length = 0;
        free(str);
        str = nullptr;
    }

    int len() const { return length; }
};
