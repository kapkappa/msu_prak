#include "field.h"

#include "omp.h"

#include <math.h>

#include <sys/time.h>

static inline double analytical(double x, double y, double z, double Lx, double Ly, double Lz, double t) {
    double a = M_PI * std::sqrt(9.0 / (Lx * Lx) + 4.0 / (Ly * Ly) + 4.0 / (Lz * Lz));
    return std::sin(3.0 * M_PI * x / Lx) *
           std::sin(2.0 * M_PI * y / Ly) *
           std::sin(2.0 * M_PI * z / Lz) *
           std::cos(a * t + 4.0 * M_PI);
}

static inline double timer() {
    struct timeval tp;
    struct timezone tzp;

    gettimeofday(&tp, &tzp);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}

static void show_help() {
    std::cout << "Need args: L N dt max_timesteps" << std::endl;
}

static double calc_residual(const Field& U_appr, const Field& U_anal, int N) {
    double max_norm = 0.0;
    for (int i = 0; i < N+1; i++) {
        for (int j = 0; j < N+1; j++) {
            for (int k = 0; k < N+1; k++) {
                double tmp = std::abs(U_appr(i,j,k) - U_anal(i,j,k));
                max_norm = std::max(tmp, max_norm);
            }
        }
    }
    return max_norm;
}


int main(int argc, char** argv) {

    if (argc != 5) {
        show_help();
        exit(0);
    }

//    auto nthreads = omp_get_max_threads();
//    omp_set_num_threads(nthreads);
    std::cout << "Threads number: " << omp_get_max_threads() << std::endl;

    int type = atoi(argv[1]);
    double L = type ? (double)type : M_PI;

    int N = atoi(argv[2]);

    double dt = std::stod(argv[3]);

    int K = atoi(argv[4]);

    double Lx = L, Ly = L, Lz = L;

    double dx = Lx / (double)N;
    double dy = Ly / (double)N;
    double dz = Lz / (double)N;

    double t = 0.0;

    Field U_prev, U_curr, U_next;

    U_prev.init(N+1, N+1, N+1);
    U_curr.init(N+1, N+1, N+1);
//    U_anal.init(N+1, N+1, N+1);


    double t1 = timer();

    // Calc U0
#pragma omp parallel for
    for (int i = 0; i < N+1; i++) {
        for (int j = 0; j < N+1; j++) {
            for (int k = 0; k < N+1; k++) {
                U_prev(i,j,k) = analytical(i*dx, j*dy, k*dz, Lx, Ly, Lz, t);
            }
        }
    }
    t += dt;

    // Calc U1
#pragma omp parallel for
    for (int i = 0; i < N+1; i++) {
        for (int j = 0; j < N+1; j++) {
            for (int k = 0; k < N+1; k++) {
                if (i == 0 || i == N || j == 0 || j == N || k == 0 || k == N) {
                    U_curr(i,j,k) = analytical(i*dx, j*dy, k*dz, Lx, Ly, Lz, t);
                } else {
                    double laplas = (U_prev(i-1,j,k) - 2 * U_prev(i,j,k) + U_prev(i+1,j,k)) / (dx * dx) +
                                    (U_prev(i,j-1,k) - 2 * U_prev(i,j,k) + U_prev(i,j+1,k)) / (dy * dy) +
                                    (U_prev(i,j,k-1) - 2 * U_prev(i,j,k) + U_prev(i,j,k+1)) / (dz * dz);

                    U_curr(i,j,k) = U_prev(i,j,k) + dt * dt * laplas / 2.0;
                }
            }
        }
    }
    t += dt;

// Main cycle
    for (int step = 0; step < K; step++) {

#pragma omp parallel for
        for (int i = 1; i < N; i++) {
            for (int j = 0; j < N+1; j++) {
                for (int k = 0; k < N+1; k++) {
                    double x_diff, y_diff, z_diff;
                    x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);

                    if (j == 0 || j == N) {
                        y_diff = U_curr(i,N-1,k) - 2 * U_curr(i,j,k) + U_curr(i,1,k);
                    } else {
                        y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    }

                    if (k == 0 || k == N) {
                        z_diff = U_curr(i,j,N-1) - 2 * U_curr(i,j,k) + U_curr(i,j,1);
                    } else {
                        z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);
                    }

                    double laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);

                    // Use U_prev instead of U_next and then just swap pointers
                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }
            }
        }

        std::swap(U_prev.array, U_curr.array);

        t += dt;
    }
/*
#pragma omp parallel for
        for (int i = 0; i < N+1; i++) {
            for (int j = 0; j < N+1; j++) {
                for (int k = 0; k < N+1; k++) {
                    U_anal(i,j,k) = analytical(i*dx, j*dy, k*dz, Lx, Ly, Lz, t);
                }
            }
    }
    std::cout << calc_residual(U_prev, U_anal, N) << std::endl;
*/

    double max_norm = 0.0;

#pragma omp parallel for reduction(max:max_norm)
    for (int i = 0; i < N+1; i++) {
        for (int j = 0; j < N+1; j++) {
            for (int k = 0; k < N+1; k++) {
                double val = analytical(i*dx, j*dy, k*dz, Lx, Ly, Lz, t-dt);
                double tmp = std::abs(U_curr(i,j,k) - val);
                max_norm = std::max(tmp, max_norm);
            }
        }
    }

    double t2 = timer();

    std::cout << "Max residual: " << max_norm << std::endl;
    std::cout << "Time: " << t2 - t1 << std::endl;

    return 0;
}

