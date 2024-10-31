#include "field.h"

#include <math.h>

#include <sys/time.h>

static inline double analytical(double x, double y, double z, int Lx, int Ly, int Lz, double t) {
    double a = M_PI * std::sqrt(9.0 / (Lx * Lx) + 4.0 / (Ly * Ly) + 4.0 / (Lz * Lz));
    return std::sin(3.0 * M_PI * x / (double)Lx) *
           std::sin(2.0 * M_PI * y / (double)Ly) *
           std::sin(2.0 * M_PI * z / (double)Lz) *
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
    double max_norm = 0.0, l2_norm = 0.0;
    for (int i = 0; i < N+1; i++) {
        for (int j = 0; j < N+1; j++) {
            for (int k = 0; k < N+1; k++) {
                double tmp = std::abs(U_appr(i,j,k) - U_anal(i,j,k));
                max_norm = std::max(tmp, max_norm);
                l2_norm += tmp * tmp;
            }
        }
    }
    return max_norm;
//    return std::sqrt(l2_norm);
}


int main(int argc, char** argv) {

    if (argc != 5) {
        show_help();
        exit(0);
    }

    int L = atoi(argv[1]);
//    double L = M_PI;

    int N = atoi(argv[2]);

    double dt = std::stod(argv[3]);
//    double T = std::stod(argv[3]);
//    double dt = T / K;

    int K = atoi(argv[4]);

    int Lx = L, Ly = L, Lz = L;

    double dx = (double)Lx / (double)N;
    double dy = (double)Ly / (double)N;
    double dz = (double)Lz / (double)N;

    double t = 0.0;

    Field U_prev, U_curr, U_next, U_anal;

    U_prev.init(N+1, N+1, N+1);
    U_curr.init(N+1, N+1, N+1);
    U_anal.init(N+1, N+1, N+1);


    // Calc U0
    for (int i = 0; i < N+1; i++) {
        for (int j = 0; j < N+1; j++) {
            for (int k = 0; k < N+1; k++) {
                double val = analytical(i*dx, j*dy, k*dz, Lx, Ly, Lz, t);
                U_prev(i,j,k) = val;
            }
        }
    }
    t += dt;
//    U_prev.print("U0");

    // Calc U1
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
//    U_curr.print("U1");


    // Steps

    double t1 = timer();

    for (int step = 0; step < K; step++) {

        for (int i = 1; i < N; i++) {
            for (int j = 0; j < N+1; j++) {
                for (int k = 0; k < N+1; k++) {
                    double x_diff, y_diff, z_diff;
                    x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);

                    if (j == 0) {
                        y_diff = U_curr(i,N-1,k) - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    } else if (j == N) {
                        y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + U_curr(i,0+1,k);
                    } else {
                        y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    }

                    if (k == 0) {
                        z_diff = U_curr(i,j,N-1) - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);
                    } else if (k == N) {
                        z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + U_curr(i,j,0+1);
                    } else {
                        z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);
                    }

                    double laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);

                    // Use U_prev instead of U_next and then just swap pointers
                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }
            }
        }

        for (int i = 0; i < N+1; i++) {
            for (int j = 0; j < N+1; j++) {
                for (int k = 0; k < N+1; k++) {
                    double val = analytical(i*dx, j*dy, k*dz, Lx, Ly, Lz, t);
                    U_anal(i,j,k) = val;
                }
            }
        }
        std::cout << calc_residual(U_prev, U_anal, N) << std::endl;

//        U_anal.print("anal");
//        U_next.print("next");

        std::swap(U_prev.array, U_curr.array);
//        std::swap(U_curr.array, U_next.array);

        t += dt;
    }

    double t2 = timer();

    std::cout << "Time: " << t2 - t1 << std::endl;

    return 0;
}

