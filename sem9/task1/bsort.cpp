#include "bsort.h"

#include <iostream>
#include <cmath>
#include <vector>

std::vector<std::pair<int, int>> comparators;

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Wrong number of arguments" << std::endl;
        exit(1);
    }

    int size = atoi(argv[1]);
    std::vector<int> array(size);

    std::cout << size << ' ' << 0 << ' ' << 0 << std::endl;

    B<int>(array, 0, 1, size);

    for (const auto& elem : comparators) {
        std::cout << elem.first << ' ' << elem.second << std::endl;
    }

    std::cout << comparators.size() << std::endl;

    int res = calc_tacts(comparators);
    std::cout << res << std::endl;

    return 0;
}
