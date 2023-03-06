#include <fstream>
#include <sys/time.h>
#include <omp.h>
#include <complex>
#include <array>
#include <vector>
#include <iostream>

using namespace std::complex_literals;
int nthreads = 1;

const double HW = 100; // h = 1 w = 1 ???????????
const double G = 0.01;

#pragma omp declare reduction(+:std::complex<double>:omp_out += omp_in) initializer( omp_priv = omp_orig)

static inline double prob(std::complex<double> z) {
    return std::abs(real(z * std::conj(z)));
}

static inline double timer() {
    struct timeval tp;
    struct timezone tzp;

    gettimeofday(&tp, &tzp);
//    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
    return omp_get_wtime();
}

struct basic_state {
    int ph1, ph2, o1su, o1sd, o2su, o2sd;

    basic_state(int _ph1, int _ph2, int _o1su, int _o1sd, int _o2su, int _o2sd) : ph1(_ph1), ph2(_ph2), o1su(_o1su), o1sd(_o1sd), o2su(_o2su), o2sd(_o2sd) {}
};

std::vector<basic_state> states = {
    {0,0,1,1,0,0},
    {0,0,0,1,0,1},
    {0,0,1,0,1,0},
    {0,0,1,0,0,1},
    {0,0,0,1,1,0},
    {0,0,0,0,1,1},

    {0,1,1,1,0,0},
    {0,1,0,1,0,1},
    {0,1,1,0,1,0},
    {0,1,1,0,0,1},
    {0,1,0,1,1,0},
    {0,1,0,0,1,1},

    {1,0,1,1,0,0},
    {1,0,0,1,0,1},
    {1,0,1,0,1,0},
    {1,0,1,0,0,1},
    {1,0,0,1,1,0},
    {1,0,0,0,1,1},

    {1,1,1,1,0,0},
    {1,1,0,1,0,1},
    {1,1,1,0,1,0},
    {1,1,1,0,0,1},
    {1,1,0,1,1,0},
    {1,1,0,0,1,1}
};

static double get_Hij(int i, int j) {
    if (i == j)
        return (states[i].o2su + states[i].o2sd + states[i].ph1 + states[i].ph2) * HW; // number of photones + numbers pf excited electrons,
                                                                                       // which means that they swallow photones

    if (states[i].o1sd == states[j].o1sd && states[i].o2sd == states[j].o2sd && states[i].ph2 == states[j].ph2) {

        if (states[i].o1su - states[j].o1su == 1 && states[j].o2su - states[i].o2su == 1 && states[i].ph1 - states[j].ph1 == 1)
            return sqrt(std::max(states[i].ph1, states[j].ph1)) * G;

        if (states[i].o1su - states[j].o1su == -1 && states[j].o2su - states[i].o2su == -1 && states[i].ph1 - states[j].ph1 == -1)
            return sqrt(std::max(states[i].ph1, states[j].ph1)) * G;

    } else if (states[i].o1su == states[j].o1su && states[i].o2su == states[j].o2su && states[i].ph1 == states[j].ph1) {

        if (states[i].o1sd - states[j].o1sd == 1 && states[j].o1sd - states[i].o1sd == 1 && states[i].ph2 - states[j].ph2 == 1)
            return sqrt(std::max(states[i].ph2, states[j].ph2)) * G;

        if (states[i].o1sd - states[j].o1sd == -1 && states[j].o1sd - states[i].o1sd == -1 && states[i].ph2 - states[j].ph2 == -1)
            return sqrt(std::max(states[i].ph2, states[j].ph2)) * G;
    }

    return 0;
}

int main(int argc, char** argv) {

    if (argc != 3) {
        std::cout << "Wrong number of arguments" << std::endl;
        return 0;
    }

    nthreads = omp_get_max_threads();
    omp_set_num_threads(nthreads);
    std::cout << "Threads number: " << omp_get_max_threads() << std::endl;


    int max_time = atoi(argv[1]);
    int max_iters = atoi(argv[2]);

    double time_step = (double) max_time / max_iters;

    double time = 0.0;

    std::ofstream file;
    file.open("log");

    std::cout << "max time: " << max_time << std::endl;
    std::cout << "max iters: " << max_iters << std::endl;
    std::cout << "time step: " << time_step << std::endl;

    file << max_time << std::endl;
    file << max_iters << std::endl;
    file << time_step << std::endl;

//    std::cout << get_Hij(23, 23) << std::endl;

    std::array<std::complex<double>, 24> psi_old = {0, 24};

// basic values
    psi_old[4] = 0.5;
    psi_old[0] = -0.5;
    psi_old[5] = 0.5;
    psi_old[3] = -0.5;

//    psi_old[3] = 1;
//    psi_old[0] = 1.0; - test 2

    std::array<std::complex<double>, 24> psi_new = {0, 24};

    double t1 = timer();

    for (int k = 0; k < max_iters; k++) {
        std::complex<double> sum = 0;

#pragma omp parallel for shared(psi_new, psi_old) reduction(+:sum)
        for (int i = 0; i < 24; i++) {
            psi_new[i] = 0;

            for (int j = 0; j < 24; j++) {
                psi_new[i] += ((std::complex<double>)(i==j) - 1i * get_Hij(i, j) * time_step) * psi_old[j];
            }

            sum += psi_new[i] * std::conj(psi_new[i]);
        }

        double result = prob(psi_old[0]) + prob(psi_old[6]) + prob(psi_old[12]) + prob(psi_old[18]);

        file << time << ' ' << result << std::endl;

        time += time_step;

        for (int i = 0; i < 24; i++)
            psi_old[i] = psi_new[i] / sqrt(sum);

    }

    double t2 = timer();

    std::cout << "calculation time: " << t2-t1 << std::endl;

    return 0;
}
