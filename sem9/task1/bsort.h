#include <iostream>
#include <vector>
#include <utility>

extern std::vector<std::pair<int, int>> comparators;

template<typename T>
void comparator(std::vector<T>& array, int a, int b) {
    if (array[a] > array[b])
        std::swap(array[a], array[b]);
}

template <typename T>
void S(std::vector<T>&, int, int, int, int, int);

template <typename T>
void B(std::vector<T>& array, int first, int step, int count) {
//    std::cout << "first: " << first << " step: " << step << " count: " << count << std::endl;

    if (count < 2)
        return;

    if (count == 2) {
        comparator<T>(array, first, first + step);
        comparators.push_back(std::make_pair(first, first + step));
        return;
    }

    int count1 = (count + 1) / 2;

    B<T>(array, first, step, count1);

    B<T>(array, first + step*count1, step, count - count1);

    S<T>(array, first, first + step*count1, step, count1, count - count1);
}

template <typename T>
void S(std::vector<T>& array, int a, int b, int d, int n, int m) {
//    std::cout << "a: " << a << " b: " << b << " d: " << d << " n: " << n << " m: " << m << std::endl;

    int n1, m1, i;

    if (n*m < 1)
        return;

    if (n == 1 && m == 1) {
        comparator<T>(array, a, b);
        comparators.push_back(std::make_pair(a, b));
        return;
    }

    n1 = n - n / 2;
    m1 = m - m / 2;
    S<T>(array, a, b, 2*d, n1, m1);
    S<T>(array, a + d, b + d, 2*d, n - n1, m - m1);

    for (i = 1; i < n-1; i += 2) {
        comparator<T>(array, a+d*i, a+d*(i+1));
        comparators.push_back(std::make_pair(a+d*i, a+d*(i+1)));
    }

    if (n % 2 == 0) {
        comparator<T>(array, a+d*(n-1), b);
        comparators.push_back(std::make_pair(a+d*(n-1), b));
        i = 1;
    } else {
        i = 0;
    }

    for (; i < m-1; i += 2) {
        comparator<T>(array, b+d*i, b+d*(i+1));
        comparators.push_back(std::make_pair(b + d*i, b + d*(i+1)));
    }
}
