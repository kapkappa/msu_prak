#include "field.h"

#include "omp.h"

#include <math.h>
#include <cstring>

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


int main(int argc, char** argv) {

    int argc_indx = 0;
    double L = 1.0;
    int N = 128;
    int K = 20;
    double dt = 1.e-5;
//    int num_threads = 1;
    int num_threads = omp_get_max_threads();

    while (argc_indx < argc) {
        if (!strcmp(argv[argc_indx], "-domain")) {
            argc_indx++;
            L = atof(argv[argc_indx]);
        } else if (!strcmp(argv[argc_indx], "-nodes")) {
            argc_indx++;
            N = atoi(argv[argc_indx]);
        } else if (!strcmp(argv[argc_indx], "-steps")) {
            argc_indx++;
            K = atoi(argv[argc_indx]);
        } else if (!strcmp(argv[argc_indx], "-dt")) {
            argc_indx++;
            dt = atof(argv[argc_indx]);
        } else if (!strcmp(argv[argc_indx], "-threads")) {
            argc_indx++;
            num_threads = atoi(argv[argc_indx]);
        } else if (!strcmp(argv[argc_indx], "-help")) {
            printf("Usage: ./prog -domain L -nodes N -steps K -dt dt -threads t\n");
            return 0;
        } else {
            argc_indx++;
        }
    }

    omp_set_num_threads(num_threads);

    double Lx = L, Ly = L, Lz = L;

    double dx = Lx / (double)(N-1);
    double dy = Ly / (double)(N-1);
    double dz = Lz / (double)(N-1);

    double t = 0.0;

    Field U_prev, U_curr, U_next;

    U_prev.init(N, N, N);
    U_curr.init(N, N, N);

    double t1 = timer();

    // Calc U0 and U1
#pragma omp parallel for
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                U_prev(i,j,k) = analytical(i*dx, j*dy, k*dz, Lx, Ly, Lz, t);
                U_curr(i,j,k) = analytical(i*dx, j*dy, k*dz, Lz, Ly, Lz, t+dt);
            }
        }
    }
    t += dt; // after step 0
    t += dt; // after step 1

// Main cycle
    for (int step = 0; step < K; step++) {

#pragma omp parallel for
        for (int i = 1; i < N-1; i++) {
            for (int j = 0; j < N; j++) {
                for (int k = 0; k < N; k++) {
                    double x_diff, y_diff, z_diff;
                    x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);

                    if (j == 0 || j == N-1) {
                        y_diff = U_curr(i,N-2,k) - 2 * U_curr(i,j,k) + U_curr(i,1,k);
                    } else {
                        y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    }

                    if (k == 0 || k == N-1) {
                        z_diff = U_curr(i,j,N-2) - 2 * U_curr(i,j,k) + U_curr(i,j,1);
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

    double max_err = 0.0;

#pragma omp parallel for reduction(max:max_err)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                double val = analytical(i*dx, j*dy, k*dz, Lx, Ly, Lz, t-dt);
                double tmp = std::abs(U_curr(i,j,k) - val);
                max_err = std::max(tmp, max_err);
            }
        }
    }

    double t2 = timer();

    printf("\n ===================================\n\n");
    printf(" Threads:    %d\n", num_threads);
    printf(" Domain:     %1.2f x %1.2f x %1.2f\n", Lx, Ly, Lz);
    printf(" Nodes:      %d x %d x %d\n", N, N, N);
    printf(" Time steps: %d\n", K);
    printf(" Time step:  %1.1E\n", dt);
    printf(" Time:       %8.6lf sec\n", t2-t1);
    printf(" Error:      %1.12E\n", max_err);

    printf("\n ===================================\n");

    return 0;
}
