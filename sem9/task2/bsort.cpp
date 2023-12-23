#include <iostream>
#include <vector>
#include <algorithm>
#include <cfloat>
#include "mpi.h"

#include <sys/time.h>
static inline double timer() {
    struct timeval tp;
    struct timezone tzp;

    gettimeofday(&tp, &tzp);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}

int rank, world_size;
int tacts_count = 0;
bool reverse = false;


template <typename T>
void SS(std::vector<T>& array, int pos1, int pos2) {
    if (rank != pos1 && rank != pos2) return;

    size_t arr_size = array.size();
    int recv_tacts;
    std::vector<T> recv_array(arr_size);
    std::vector<T> result_array(arr_size);

    int par;
    if (rank == pos1) {
        par = pos2;
    } else {
        par = pos1;
    }

    MPI_Sendrecv(&tacts_count, 1, MPI_INT, par, 0,
                 &recv_tacts, 1, MPI_INT, par, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    tacts_count = std::max(tacts_count, recv_tacts) + 1;

    MPI_Sendrecv(array.data(), arr_size, MPI_DOUBLE, par, 1,
                 recv_array.data(), arr_size, MPI_DOUBLE, par, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    if (rank == pos2) {
        for (int i = arr_size - 1, j = arr_size - 1, k = arr_size - 1; k >= 0;) {
            if (reverse ? array[i] < recv_array[j] : array[i] > recv_array[j]) {
                result_array[k--] = array[i--];
            } else {
                result_array[k--] = recv_array[j--];
            }
        }
    } else {
        for (int i = 0, j = 0, k = 0; k < arr_size;) {
            if (reverse ? array[i] > recv_array[j] : array[i] < recv_array[j]) {
                result_array[k++] = array[i++];
            } else {
                result_array[k++] = recv_array[j++];
            }
        }
    }

    for (int i = 0; i < arr_size; i++)
        array[i] = result_array[i];

    return;
}


template <typename T>
void S(std::vector<T>& array, int first1, int first2, int step, int count1, int count2) {

    int n1, m1, i;

    if (count1 * count2 < 1)
        return;

    if (count1 == 1 && count2 == 1) {
        SS<T>(array, first1, first2);
        return;
    }

    n1 = count1 / 2;
    m1 = count2 / 2;
    S<T>(array, first1, first2, 2*step, count1 - n1, count2 - m1);
    S<T>(array, first1 + step, first2 + step, 2*step, count1 - n1, count2 - m1);

    for (i = 1; i < count1 - 1; i += 2) {
        SS<T>(array, first1 + step * i, first1 + step * (i+1));
    }

    if (count1 % 2 == 0) {
        SS<T>(array, first1 + step * (count1 - 1), first2);
        i = 1;
    } else {
        i = 0;
    }

    for (; i < count2 - 1; i += 2) {
        SS<T>(array, first2 + step * i, first2 + step * (i+1));
    }
}


template <typename T>
void B(std::vector<T>& array, int first, int step, int count) {
    if (count < 2) {
        return;
    }
    int mid = count / 2;
    B<T>(array, first, step, mid);
    B<T>(array, first + mid, step, count - mid);
    S<T>(array, first, first + mid, step, mid, count - mid);
}


template <typename T>
void print(const std::vector<T>& array) {
    for (int i = 0; i < world_size; i++) {
        if (rank == i) {
            std::cout << "rank: " << rank;
            for (const auto& elem : array)
                std::cout << ' ' << elem;
            std::cout << std::endl;
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}


int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Enter the size of array and direction of sorting ({0, <}, {1, >})!" << std::endl;
        return 1;
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int size = atoi(argv[1]);
    reverse = atoi(argv[2]);

    int local_size = (size + world_size - 1) / world_size;
    int extra = size % world_size;

    std::vector<double> array(local_size);

    srand(rank * 100);

    for (int i = 0; i < local_size; i++) {
        array[i] = rand() % 10000;
        if (rank >= extra && i >= size / world_size) {
            array[i] = reverse ? DBL_MAX : -DBL_MAX;
        }
    }

//    print(array);

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = timer();

    if (reverse)
        std::sort(array.begin(), array.end(), std::greater<double>());
    else
        std::sort(array.begin(), array.end());
    B(array, 0, 1, world_size);

    double t1 = timer();

//    print(array);

    int max_tacts;
    MPI_Reduce(&tacts_count, &max_tacts, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (!rank) {
        std::cout << "tacts: " << max_tacts << std::endl;
        std::cout << "time: " << t1-t0 << std::endl;
    }

    MPI_Finalize();
    return 0;
}
