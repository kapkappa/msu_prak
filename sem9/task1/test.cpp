#include "bsort.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#include <sys/time.h>
static inline double timer() {
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}

std::vector<std::pair<int, int>> comparators;

template <typename T>
void print(const std::vector<T>& array, const std::string str = "") {
    std::cout << str;
    for (const auto& elem : array)
        std::cout << elem << ' ';
    std::cout << std::endl;
}


int main(int argc, char** argv) {
    for (size_t size = 1; size <= 24; size++) {
        std::cout << "size: " << size;
        double t1 = timer();

        std::vector<bool> start_seq(size);
        std::vector<bool> seq(size);

        for (size_t i = 0; i <= size; i++) {

            std::fill(start_seq.begin(), start_seq.begin() + i, 1);

            do {
                seq = start_seq;
                B<bool>(seq, 0, 1, size);
                comparators.clear();
                if (not std::is_sorted(seq.begin(), seq.end())) {
                    print<bool>(seq, "Not sorted seq: ");
                    std::cerr << "Error" << std::endl;
                    exit(2);
                }
            } while(std::prev_permutation(start_seq.begin(), start_seq.end()));
        }

        double t2 = timer();
        std::cout << "\t time: " << t2-t1 << std::endl;
    }
    return 0;
}
