#include <complex>
#include <array>
#include <vector>
#include <iostream>

const double HW = 1;
const double G = 0.01;

int number_of_atoms = 2;
bool interaction = true;
double frequency_atom = 0.0;
double frequency_photon = 0.0;
int min_number_of_photons = 0;
int max_number_of_photons = 2;
double d_time = 0.001;
double HPLANK = 6.62606957e-27;
int steps = 10;

double prob(std::complex<double> z) {
    return std::abs(real(z * std::conj(z)));
}

struct basic_state {
    bool ph1, ph2, o1su, o1sd, o2su, o2sd;

    basic_state(bool _ph1, bool _ph2, bool _o1su, bool _o1sd, bool _o2su, bool _o2sd) : ph1(_ph1), ph2(_ph2), o1su(_o1su), o1sd(_o1sd), o2su(_o2su), o2sd(_o2sd) {}
};

std::vector<basic_state> states = {
    {0, 0, 1, 1, 0, 0},
    {0, 0, 1, 1, 0, 1},
    {0, 0, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1},

    {0, 1, 0, 1, 0, 0},
    {0, 1, 0, 1, 0, 1},
    {0, 1, 0, 1, 1, 0},
    {0, 1, 0, 1, 1, 1},

    {0, 1, 1, 0, 0, 0},
    {0, 1, 1, 0, 0, 1},
    {0, 1, 1, 0, 1, 0},
    {0, 1, 1, 0, 1, 1},

    {1, 0, 0, 1, 0, 0},
    {1, 0, 0, 1, 0, 1},
    {1, 0, 0, 1, 1, 0},
    {1, 0, 0, 1, 1, 1},

    {1, 0, 1, 0, 0, 0},
    {1, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 1, 0},
    {1, 0, 1, 0, 1, 1},

    {1, 1, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 1},
    {1, 1, 0, 0, 1, 0},
    {1, 1, 0, 0, 1, 1}
};

double get_Hij(int i, int j) {
    if (i == j)
        return (states[i].o2su + states[i].o2sd + states[i].ph1 + states[i].ph2) * HW; // number of photones + numbers pf excited electrons,
                                                                                       // which means that they swallow photones
    if (states[i].o1su == states[j].o1su && states[i].o2su == states[j].o2su && states[i].ph2 == states[j].ph2) {

        if(states[i].o1su - states[j].o1su == 1 && states[j].o1su - states[i].o1su == 1 && states[i].ph1 - states[j].ph1 == 1)
            return std::sqrt(std::max(states[i].ph1, states[j].ph1)) * G;

        if(states[i].o1su - states[j].o1su == -1 && states[j].o1su - states[i].o1su == -1 && states[i].ph1 - states[j].ph1 == -1)
            return std::sqrt(std::max(states[i].ph1, states[j].ph1)) * G;

    } else if (states[i].o1su == states[j].o1su && states[i].o2su == states[j].o2su && states[i].ph2 == states[j].ph2) {

        if(states[i].o1sd - states[j].o1sd == 1 && states[j].o2sd - states[i].o2sd == 1 && states[i].ph2 - states[j].ph2 == 1)
            return std::sqrt(std::max(states[i].ph2, states[j].ph2)) * G;

        if(states[i].o1sd - states[j].o1sd == -1 && states[j].o2sd - states[i].o2sd == -1 && states[i].ph2 - states[j].ph2 == -1)
            return std::sqrt(std::max(states[i].ph2, states[j].ph2)) * G;
    }

    return 0;
}

int main(int argc, char** argv) {

    if (argc != 3) {
        std::cout << "Wrong number of arguments" << std::endl;
        return 0;
    }

    int max_time = atoi(argv[1]);
    int max_iters = atoi(argv[2]);

    double time_step = (double) max_time / max_iters;

    double time = 0.0;

    std::cout << "time step: " << time_step << std::endl;

    std::cout << get_Hij(23, 23) << std::endl;

    std::array<std::complex<double>, 24> psi_old = {0, 24};

    psi_old[4] = 0.5;
    psi_old[0] = -0.5;
    psi_old[5] = 0.5;
    psi_old[3] = -0.5;

    std::array<std::complex<double>, 24> psi_new = {0, 24};

    for (int k = 0; k < max_iters; k++) {
        std::complex<double> sum = 0;

        for (int i = 0; i < 24; i++) {
            psi_new[i] = 0;

            for (int j = 0; j < 24; j++) {
                psi_new[i] += ((i==j) - (std::complex<double>) 1i * get_Hij(i, j) * time_step) * psi_old[j];
            }

            sum += psi_new[i] * std::conj(psi_new[i]);
        }

        double result = prob(psi_old[0] + prob(psi_old[6]) + prob(psi_old[12]) + prob(psi_old[18]));

        std::cout << t << ' ' << result << std::endl;

    }

    return 0;
}
