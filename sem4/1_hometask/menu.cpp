#include <iostream>
#include <limits>
#include "string.h"

using namespace std;

void main_menu() {
    cout << "\nEnter key to choose group:" << endl
    << "1 - Show an example" << endl
    << "2 - Constructors" << endl
    << "3 - Comparing" << endl
    << "4 - Changing" << endl
    << "5 - Output" << endl
    << "0 - Exit" << endl;
}

int main() {
    main_menu();

    int key;
    cin >> key;
    while(key) {
        switch (key) {
            case 0: {
                cout << "Bye!" << endl;
                return 0;
            }
            case 2: {
                cout << "\nEnter key to choose constructor:" << endl
                << "1 - default" << endl
                << "2 - char" << endl
                << "3 - int" << endl
                << "4 - String, copy constructor" << endl
                << "5 - std::string" << endl
                << "0 - to main menu" << endl;

                int key1 = 0;
                cin >> key1;
                if (key1 == 0) {
                    key = -1;
                } else if (key1 == 1) {
                    String s;
                    cout << "String s created, length: " << s.len() << "  str = nullptr" << endl;
                } else if (key1 == 2) {
                    cout << "Enter char:" << endl;
                    char c;
                    cin >> c;
                    String s(c);
                    cout << "String s created from char: " << s << "  length: " << s.len() << endl;
                } else if (key1 == 3) {
                    cout << "Enter int:" << endl;
                    int i;
                    cin >> i;
                    String s(i);
                    cout << "String s created from int: " << s << "  length: " << s.len() << endl;
                } else if (key1 == 4) {
                    cout << "You have chosen copy constructor" << endl;
                    String a = std::string("first test string");
                    String b = std::string("second test string\nwith extra line");
                    String c = std::string("donate to 4274-3200-3165-8787 Sberbank");
                    String d;
                    cout << "Here we have 4 test String objects:" << endl
                    << "First: " << a << endl
                    << "Second: " << b << endl
                    << "Third: " << c << endl
                    << "Fourth: " << d << "\t yeah, this one is empty" << endl
                    << "\nNow lets check constructor:" << endl;
                    String s1(a);
                    cout << "String s1 created from String a: " << s1 << "  length: " << s1.len() << endl;
                    String s2(b);
                    cout << "String s2 created from String b: " << s2 << "  length: " << s2.len() << endl;
                    String s3(c);
                    cout << "String s3 created from String c: " << s3 << "  length: " << s3.len() << endl;
                    String s4(d);
                    cout << "String s4 created from String d: " << s4 << "  length: " << s4.len() << endl;
                } else if (key1 == 5) {
                    cout << "Enter a string:" << endl;
                    std::string str;
                    cin.ignore();
                    std::getline(cin, str);
                    String s(str);
                    cout << "String s created from string: " << s << "  length: " << s.len() << endl;
                } else {
                    cout << "Incorrect key, try again!" << endl;
                }
                break;
            }
            case 3: {
                cout << "\n Enter key to choose comparison:" << endl
                << "1 - <" << endl
                << "2 - <=" << endl
                << "3 - >" << endl
                << "4 - >=" << endl
                << "5 - ==" << endl
                << "6 - !=" << endl
                << "0 - to main menu" << endl;

                int key3;
                cin >> key3;
                if (key3 == 0) {
                    key = -1;
                    break;
                }
                String s1, s2;
                cout << "Enter first String" << endl;
                cin.ignore();
                cin >> s1;
                cout << "Enter second String" << endl;
                cin >> s2;

                if (key3 == 1) {
                    cout << "s1 < s2 ?: " << (s1 < s2) << endl;
                } else if (key3 == 2) {
                    cout << "s1 <= s2 ?: " << (s1 <= s2) << endl;
                } else if (key3 == 3) {
                    cout << "s1 > s2 ?: " << (s1 > s2) << endl;
                } else if (key3 == 4) {
                    cout << "s1 >= s2 ?: " << (s1 >= s2) << endl;
                } else if (key3 == 5) {
                    cout << "s1 == s2 ?: " << (s1 == s2) << endl;
                } else if (key3 == 6) {
                    cout << "s1 != s2 ?: " << (s1 != s2) << endl;
                } else {
                    cout << "Incorrect key, try again!" << endl;
                }
                break;
            }
            case 4: {
                std::cout << "Enter String, you want to change:" << std::endl;
                String s;
                cin.ignore();
                cin >> s;
                cout << "Now, choose the way how to change it: " << endl
                << "1 - add an element to the end" << endl
                << "2 - pop an element from the end" << endl
                << "3 - add a String (concatenation)" << endl
                << "4 - change an indexed element" << endl
                << "5 - assign another string (operator=)" << endl
                << "6 - erase String" << endl
                << "0 - go back to menu" << endl;

                int key4 = 0;
                cin >> key4;
                if (key4 == 0) {
                    key = -1;
                } else if (key4 == 1) {
                    cout << "Enter a symbol" << endl;
                    cin.ignore();
                    char c = getchar();
                    s.push_back(c);
                    cout << "Now String looks like: " << s << endl;
                } else if (key4 == 2) {
                    s.pop_back();
                    cout << "Now String looks like: " << s << endl;
                } else if (key4 == 3) {
                    cout << "Enter a String" << endl;
                    String tmp;
                    cin.ignore();
                    cin >> tmp;
                    s += tmp;
                    cout << "Now String looks like: " << s << endl;
                } else if (key4 == 4) {
                    cout << "Enter a positin" << endl;
                    int i;
                    cin >> i;
                    cout << "Enter a new symbol" << endl;
                    cin.ignore();
                    char c = getchar();
                    s[i] = c;
                    cout << "Now String looks like: " << s << endl;
                } else if (key4 == 5) {
                    cout << "Enter a String" << endl;
                    String s2;
                    cin.ignore();
                    cin >> s2;
                    s = s2;
                    cout << "Now String looks like: " << s << endl;
                } else if (key4 == 6) {
                    s.erase();
                    cout << "Now erasef String looks like: " << s << "  length: " << s.len() << endl;
                } else {
                    cout << "Incorrect key, try again!" << endl;
                }
                break;
            }
            case 5: {
                cout << "Enter String:" << endl;
                String s;
                cin.ignore();
                cin >> s;

                cout << "Choose output: " << endl
                << "1 - print" << endl
                << "2 - show an element" << endl
                << "3 - show length" << endl
                << "0 - back to main menu" << endl;

                int key5;
                cin >> key5;
                if (key5 == 0) {
                    key = -1;
                } else if (key5 == 1) {
                    s.print();
                } else if (key5 == 2) {
                    cout << "Enter a position" << endl;
                    int i;
                    cin >> i;
                    cout << "s[" << i << "]: " << s[i] << endl;
                } else if (key5 == 3) {
                    cout << "Length: " << s.len() << endl;
                } else {
                    cout << "Incorrect key, try again!" << endl;
                }
                break;
            }
            case -1: {
                main_menu();
                cin >> key;
                break;
            }
            case 1: {
                cout << "EXAMPLE!!!" << endl;
                cout << "Testing default constructor" << endl;
                String a1;
                cout << "a1: " << a1 << endl;
                cout << "Testing char(a) constructor" << endl;
                String a2('a');
                cout << "a2: " << a2 << endl;
                cout << "Testing string(string) constructor" << endl;
                std::string str("string");
                a1 = str;
                a1.print();
                cout << "Testing int {123, 0, -123, -0} constructor" << endl;
                String b1(123), b2(0), b3(-123), b4(-0);
                cout << "b1: "  << b1 << endl;
                cout << "b2: "  << b2 << endl;
                cout << "b3: "  << b3 << endl;
                cout << "b4: "  << b4 << endl;
                cout << "Testing concatenation, with = and +=" << endl;
                String c1 = a1+b1, c2 = a2+b2, c3 = c1+a2+3, c4 = 1+c2;
                cout << "c1 = a1+b1: " << c1 << endl;
                cout << "c2 = a2+b2: " << c2 << endl;
                cout << "c3 = a1+a1+3: " << c3 << endl;
                cout << "c4 = 1+c2: " << c4 << endl;
                String c5;
                c5=c4=c3=c2=c1;
                c5 += b1 + b2 + b3 += b4;
                cout << "c5=c4=c3=c2=c1" << endl;
                cout << "c1: " << c1 << endl
                << "c2: " << c2 << endl
                << "c3: " << c3 << endl
                << "c4: " << c4 << endl
                << "c5 += b1 + b2 + b3 += b4: " << c5 << endl;
                cout << "Testing comparison" << endl;
                cout << "a1 != a2: " << (a1 != a2) << endl;
                cout << "c1 > c2: " << (c1 > c2) << endl;
                cout << "b1 < b2: " << (b1 < b2) << endl;
                cout << "Operator [], >> and << " << endl;
                cout << "c4 :" << c4 << endl;
                c4[0] = 'S';
                cout << "c4[0] = 'S'; " << "c4: " << c4 << endl;
                cout << "Enter String:" << endl;
                String s;
                cin.ignore();
                cin >> s;
                cout << "Your String s: " << s << "  length: " << s.len() << endl;
                cout << "Testing str_to_int" << endl;
                int x1 = b1.s2int(), x2 = b2.s2int(), x3 = b3.s2int(), x4 = b4.s2int(), x5 = c1.s2int();
                cout << "x1 = b1: " << x1 << endl
                << "x2 = b2: " << x2 << endl
                << "x3 = b3: " << x3 << endl
                << "x4 = b4: " << x4 << endl
                << "x5 = c1: " << x5 << endl;
                cout << "Testing str_to_float" << endl;
                String d1 = 0, d2 = -1234, d3 = +123, d4 = std::string("0.0"), d5 = std::string("123.567"), d6 = std::string("-0,3210");
                float f1 = d1.s2float(), f2 = d2.s2float(), f3 = d3.s2float(), f4 = d4.s2float(), f5 = d5.s2float(), f6 = d6.s2float();
                cout << "d1: " << d1 << "  f1: " << f1 << endl
                << "d2: " << d2 << "  f2: " << f2 << endl
                << "d3: " << d3 << "  f3: " << f3 << endl
                << "d4: " << d4 << "  f4: " << f4 << endl
                << "d5: " << d5 << "  f5: " << f5 << endl
                << "d6: " << d6 << "  f6: " << f6 << endl;

                key = -1;
                break;
            }
            default: {
                cout << "Incorrect key, try again!" << endl;
                key = -1;
                break;
            }
        }
    }

    return 0;
}
