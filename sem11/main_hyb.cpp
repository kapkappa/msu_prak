#include "field.h"

#include <math.h>
#include <cstring>

#include <omp.h>
#include <mpi.h>

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


void calc_analytical(Field& U, int x_size, int y_size, int z_size, int* cart_coords, double dx, double dy, double dz, int Lx, int Ly, int Lz, double t) {
#pragma omp parallel for
    for (int i = 0; i < x_size; i++) {
        for (int j = 0; j < y_size; j++) {
            for (int k = 0; k < z_size; k++) {
                int x_offset = cart_coords[0] * x_size + i;
                int y_offset = cart_coords[1] * y_size + j;
                int z_offset = cart_coords[2] * z_size + k;
                U(i,j,k) = analytical(x_offset * dx, y_offset * dy, z_offset * dz, Lx, Ly, Lz, t);
            }
        }
    }
}


double max_norm(const Field& U, int x_size, int y_size, int z_size, int*cart_coords, double dx, double dy, double dz, int Lx, int Ly, int Lz, double t) {
    double max_norm = 0.0;

#pragma omp parallel for reduction(max:max_norm)
    for (int i = 0; i < x_size; i++) {
        for (int j = 0; j < y_size; j++) {
            for (int k = 0; k < z_size; k++) {
                int x_offset = cart_coords[0] * x_size + i;
                int y_offset = cart_coords[1] * y_size + j;
                int z_offset = cart_coords[2] * z_size + k;
                double val = analytical(x_offset * dx, y_offset * dy, z_offset * dz, Lx, Ly, Lz, t);
                double tmp = std::abs(U(i,j,k) - val);
                max_norm = std::max(tmp, max_norm);
            }
        }
    }

//    MPI_Allreduce(&max_norm, &max_norm, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
//    return max_norm;

    double result = 0.0;
    MPI_Reduce(&max_norm, &result, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    return result;
}


int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int rank, world_size;

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

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
            if (!rank)
                printf("Usage: ./prog_mpi -domain L -nodes N -steps K -dt dt -threads t\n");
            MPI_Finalize();
            return 0;
        } else {
            argc_indx++;
        }
    }

    omp_set_num_threads(num_threads);

    int dims[3] = {0, 0, 0};

    MPI_Dims_create(world_size, 3, dims);

    if (N % dims[0] || N % dims[1] || N % dims[2]) {
        if (!rank)
            std::cerr << "Incompatible grid size with number of processes" << std::endl;
        MPI_Finalize();
        return 1;
    }

    double Lx = L, Ly = L, Lz = L;

    double dx = Lx / (double)(N-1);
    double dy = Ly / (double)(N-1);
    double dz = Lz / (double)(N-1);

    double t = 0.0;

    int x_size = N / dims[0];
    int y_size = N / dims[1];
    int z_size = N / dims[2];

    int periods[3] = {true, true, true};
    int reorder = true;
    MPI_Comm new_communicator;
    MPI_Cart_create(MPI_COMM_WORLD, 3, dims, periods, reorder, &new_communicator);

    // My rank in the new communicator
    int cart_rank;
    MPI_Comm_rank(new_communicator, &cart_rank);

    // Get my coordinates in the new communicator
    int cart_coords[3];
    MPI_Cart_coords(new_communicator, cart_rank, 3, cart_coords);

    enum DIRECTIONS {LEFT, RIGHT, BACK, FRONT, BELOW, ABOVE};

    char* neighbours_names[6] = {"left", "right", "back", "front", "below", "above"};
    int neighbours_ranks[6];

    // Let consider dims[0] = X, so the shift tells us our left and right neighbours
    MPI_Cart_shift(new_communicator, 0, 1, &neighbours_ranks[LEFT], &neighbours_ranks[RIGHT]);

    // Let consider dims[1] = Y, so the shift tells us our back and front neighbours
    MPI_Cart_shift(new_communicator, 1, 1, &neighbours_ranks[BACK], &neighbours_ranks[FRONT]);

    // Let consider dims[2] = Z, so the shift tells us our below and above neighbours
    MPI_Cart_shift(new_communicator, 2, 1, &neighbours_ranks[BELOW], &neighbours_ranks[ABOVE]);

//    printf("[MPI process %d] I am located at (%d, %d, %d). My neighbours: %d %d, %d %d, %d %d\n", rank, cart_coords[0], cart_coords[1], cart_coords[2],
//            neighbours_ranks[0], neighbours_ranks[1], neighbours_ranks[2], neighbours_ranks[3], neighbours_ranks[4], neighbours_ranks[5]);

    Field U_prev, U_curr;

    U_prev.init(x_size, y_size, z_size);
    U_curr.init(x_size, y_size, z_size);

    double* yz_buff_prev = (double*)calloc(y_size * z_size, sizeof(double));
    double* yz_buff_next = (double*)calloc(y_size * z_size, sizeof(double));

    double* xz_buff_prev = (double*)calloc(x_size * z_size, sizeof(double));
    double* xz_buff_next = (double*)calloc(x_size * z_size, sizeof(double));

    double* xy_buff_prev = (double*)calloc(x_size * y_size, sizeof(double));
    double* xy_buff_next = (double*)calloc(x_size * y_size, sizeof(double));

    MPI_Datatype XZ;
    MPI_Type_vector(x_size, z_size, y_size * z_size, MPI_DOUBLE, &XZ);  //blocks_count, block_size, stride
    MPI_Type_commit(&XZ);

    MPI_Datatype XY;
    MPI_Type_vector(y_size * x_size, 1, z_size, MPI_DOUBLE, &XY);
    MPI_Type_commit(&XY);

    MPI_Request requests[12];

    int rank_left  = neighbours_ranks[LEFT];
    int rank_right = neighbours_ranks[RIGHT];
    int rank_back  = neighbours_ranks[BACK];
    int rank_front = neighbours_ranks[FRONT];
    int rank_below = neighbours_ranks[BELOW];
    int rank_above = neighbours_ranks[ABOVE];

    int x_prev_offset = x_size - 1;
    int x_next_offset = 0;

    int y_prev_offset = y_size - 1;
    if (cart_coords[1] == dims[1]-1)
        y_prev_offset = y_size - 2;

    int y_next_offset = 0;
    if (cart_coords[1] == 0)
        y_next_offset = 1;

    int z_prev_offset = z_size - 1;
    if (cart_coords[2] == dims[2]-1)
        z_prev_offset = z_size - 2;

    int z_next_offset = 0;
    if (cart_coords[2] == 0)
        z_next_offset = 1;

    int from = 0;
    int to = x_size;

    if (cart_coords[0] == 0)
        from = 1;
    if (cart_coords[0] == dims[0]-1)
        to = x_size-1;

    MPI_Barrier(MPI_COMM_WORLD);
    double t1 = timer();

    // Calc U0
    calc_analytical(U_prev, x_size, y_size, z_size, cart_coords, dx, dy, dz, Lx, Ly, Lz, t);
    t += dt;

    // Calc U1
    calc_analytical(U_curr, x_size, y_size, z_size, cart_coords, dz, dy, dz, Lx, Ly, Lz, t);
    t += dt;

// Main cycle
    for (int step = 0; step < K; step++) {

        MPI_Isend(U_curr.yz(x_prev_offset), y_size*z_size, MPI_DOUBLE, rank_right, 0, MPI_COMM_WORLD, &requests[0]);
        MPI_Isend(U_curr.yz(x_next_offset), y_size*z_size, MPI_DOUBLE, rank_left,  1, MPI_COMM_WORLD, &requests[2]);
        MPI_Isend(U_curr.xz(y_prev_offset), 1,             XZ,         rank_front, 2, MPI_COMM_WORLD, &requests[4]);
        MPI_Isend(U_curr.xz(y_next_offset), 1,             XZ,         rank_back,  3, MPI_COMM_WORLD, &requests[6]);
        MPI_Isend(U_curr.xy(z_prev_offset), 1,             XY,         rank_above, 4, MPI_COMM_WORLD, &requests[8]);
        MPI_Isend(U_curr.xy(z_next_offset), 1,             XY,         rank_below, 5, MPI_COMM_WORLD, &requests[10]);

        MPI_Irecv(yz_buff_prev, y_size*z_size, MPI_DOUBLE, rank_left,  0, MPI_COMM_WORLD, &requests[1]);
        MPI_Irecv(yz_buff_next, y_size*z_size, MPI_DOUBLE, rank_right, 1, MPI_COMM_WORLD, &requests[3]);
        MPI_Irecv(xz_buff_prev, x_size*z_size, MPI_DOUBLE, rank_back,  2, MPI_COMM_WORLD, &requests[5]);
        MPI_Irecv(xz_buff_next, x_size*z_size, MPI_DOUBLE, rank_front, 3, MPI_COMM_WORLD, &requests[7]);
        MPI_Irecv(xy_buff_prev, x_size*y_size, MPI_DOUBLE, rank_below, 4, MPI_COMM_WORLD, &requests[9]);
        MPI_Irecv(xy_buff_next, x_size*y_size, MPI_DOUBLE, rank_above, 5, MPI_COMM_WORLD, &requests[11]);

#pragma omp parallel for
        for (int i = 1; i < x_size-1; i++) {
            for (int j = 1; j < y_size-1; j++) {
                for (int k = 1; k < z_size-1; k++) {

                    double x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                    double y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    double z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                    double laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);

                    // Use U_prev instead of U_next and then just swap pointers
                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }
            }
        }

        MPI_Waitall(12, requests, MPI_STATUS_IGNORE);

        // i == 0 or i == x_size - 1
        {
            int i = from;
            if (i == 0) {
#pragma omp parallel for
                for (int j = 1; j < y_size-1; j++) {
                    for (int k = 1; k < z_size-1; k++) {
                        double x_diff = yz_buff_prev[j*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                        double y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                        double z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                        double laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);

                        U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                    }
                }
            }

            i = to-1;
            if (i == x_size-1) {
#pragma omp parallel for
                for (int j = 1; j < y_size-1; j++) {
                    for (int k = 1; k < z_size-1; k++) {
                        double x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + yz_buff_next[j*z_size + k];
                        double y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                        double z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                        double laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);

                        U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                    }
                }
            }
        }

        // j == 0 or j == y_size - 1
        {
            int j = 0;
#pragma omp parallel for
            for (int i = 1; i < x_size-1; i++) {
                for (int k = 1; k < z_size-1; k++) {
                    double x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                    double y_diff = xz_buff_prev[i*z_size+k] - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    double z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                    double laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);

                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }
            }

            j = y_size-1;
#pragma omp paralle for
            for (int i = 1; i < x_size-1; i++) {
                for (int k = 1; k < z_size-1; k++) {
                    double x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                    double y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + xz_buff_next[i*z_size+k];
                    double z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                    double laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);

                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }
            }
        }

        // k == 0 or k == z_size-1
        {
            int k = 0;
#pragma omp parallel for
            for (int i = 1; i < x_size-1; i++) {
                for (int j = 1; j < y_size-1; j++) {
                    double x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                    double y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    double z_diff = xy_buff_prev[i*y_size+j] - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                    double laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);

                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }
            }

            k = z_size-1;
#pragma omp parallel for
            for (int i = 1; i < x_size-1; i++) {
                for (int j = 1; j < y_size-1; j++) {
                    double x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                    double y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    double z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + xy_buff_next[i*y_size+j];

                    double laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);

                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }
            }
        }

        // Now, remaining iters: i == 0 || x_size-1 && j == 0 || y_size - 1 && k == 0 || z_size-1
        for (int i = 1; i < x_size-1; i++) {
            // j == 0 && k == 0
            {
                int j = 0;
                int k = 0;

                double x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                double y_diff = xz_buff_prev[i*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                double z_diff = xy_buff_prev[i*y_size + j] - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                double laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
            }

            // j == 0 && k == z_size-1
            {
                int j = 0;
                int k = z_size-1;

                double x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                double y_diff = xz_buff_prev[i*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                double z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + xy_buff_next[i*y_size + j];

                double laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
            }

            // j == y_size-1 && k == 0
            {
                int j = y_size-1;
                int k = 0;

                double x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                double y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + xz_buff_next[i*z_size + k];
                double z_diff = xy_buff_prev[i*y_size + j] - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                double laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
            }

            // j == y_size-1 && k == z_size-1
            {
                int j = y_size-1;
                int k = z_size-1;

                double x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                double y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + xz_buff_next[i*z_size + k];
                double z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + xy_buff_next[i*y_size + j];

                double laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
            }
        }


        // i == 0 || x_size-1 && k == 0 || z_size-1
        {
            int i, k;
            double x_diff, y_diff, z_diff, laplas;
            i = from;
            if (i == 0) {
                for (int j = 1; j < y_size-1; j++) {
                    k = 0;

                    x_diff = yz_buff_prev[j*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                    y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    z_diff = xy_buff_prev[i*y_size + j] - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                    laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }

                for (int j = 1; j < y_size-1; j++) {
                    k = z_size-1;

                    x_diff = yz_buff_prev[j*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                    y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + xy_buff_next[i*y_size + j];

                    laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }
            }

            i = to-1;
            if (i == x_size-1) {
                for (int j = 1; j < y_size-1; j++) {
                    k = 0;

                    x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + yz_buff_next[j*z_size + k];
                    y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    z_diff = xy_buff_prev[i*y_size + j] - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                    laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }

                for (int j = 1; j < y_size-1; j++) {
                    k = z_size-1;

                    x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + yz_buff_next[j*z_size + k];
                    y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + xy_buff_next[i*y_size + j];

                    laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }
            }
        }

        // i == 0 || x_size-1 && j == 0 || y_size-1
        {
            int i, j;
            double x_diff, y_diff, z_diff, laplas;
            i = from;
            if (i == 0) {
                for (int k = 1; k < z_size-1; k++) {
                    j = 0;

                    x_diff = yz_buff_prev[j*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                    y_diff = xz_buff_prev[i*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                    laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }

                for (int k = 1; k < z_size-1; k++) {
                    j = y_size-1;

                    x_diff = yz_buff_prev[j*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                    y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + xz_buff_next[i*z_size + k];
                    z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                    laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }
            }

            i = to-1;
            if (i == x_size-1) {
                for (int k = 1; k < z_size-1; k++) {
                    j = 0;

                    x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + yz_buff_next[j*z_size + k];
                    y_diff = xz_buff_prev[i*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                    laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }

                for (int k = 1; k < z_size-1; k++) {
                    j = y_size-1;

                    x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + yz_buff_next[j*z_size + k];
                    y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + xz_buff_next[i*z_size + k];
                    z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                    laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                    U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
                }
            }
        }

        // Now we need to process 8 corners
        {
            int i, j, k;
            double x_diff, y_diff, z_diff, laplas;
            i = from;
            if (i == 0) {
                j = 0;
                k = 0;

                x_diff = yz_buff_prev[j*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                y_diff = xz_buff_prev[i*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                z_diff = xy_buff_prev[i*y_size + j] - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);

                j = 0;
                k = z_size-1;

                x_diff = yz_buff_prev[j*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                y_diff = xz_buff_prev[i*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + xy_buff_next[i*y_size + j];

                laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);

                j = y_size-1;
                k = 0;

                x_diff = yz_buff_prev[j*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + xz_buff_next[i*z_size + k];
                z_diff = xy_buff_prev[i*y_size + j] - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);

                j = y_size-1;
                k = z_size-1;

                x_diff = yz_buff_prev[j*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + xz_buff_next[i*z_size + k];
                z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + xy_buff_next[i*y_size + j];

                laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
            }

            i = to-1;
            if (i == x_size-1) {

                j = 0;
                k = 0;

                x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + yz_buff_next[j*z_size + k];
                y_diff = xz_buff_prev[i*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                z_diff = xy_buff_prev[i*y_size + j] - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);

                j = 0;
                k = z_size-1;

                x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + yz_buff_next[j*z_size + k];
                y_diff = xz_buff_prev[i*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + xy_buff_next[i*y_size + j];

                laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);

                j = y_size-1;
                k = 0;

                x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + yz_buff_next[j*z_size + k];
                y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + xz_buff_next[i*z_size + k];
                z_diff = xy_buff_prev[i*y_size + j] - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);

                laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);

                j = y_size-1;
                k = z_size-1;

                x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + yz_buff_next[j*z_size + k];
                y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + xz_buff_next[i*z_size + k];
                z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + xy_buff_next[i*y_size + j];

                laplas = x_diff / (dx * dx) + y_diff / (dy * dy) + z_diff / (dz * dz);
                U_prev(i,j,k) =  dt * dt * laplas - U_prev(i,j,k) + 2 * U_curr(i,j,k);
            }
        }

        std::swap(U_prev.array, U_curr.array);

        t += dt;
    }

    double residual = max_norm(U_curr, x_size, y_size, z_size, cart_coords, dx, dy, dz, Lx, Ly, Lz, t-dt);

    MPI_Barrier(MPI_COMM_WORLD);
    double t2 = timer();

    if (!rank) {
        printf("\n ===================================\n\n");
        printf(" Processes:  %d\n", world_size);
        printf(" Grid:       %d x %d x %d\n", dims[0], dims[1], dims[2]);
        printf(" Threads:    %d\n", num_threads);
        printf(" Domain:     %1.2f x %1.2f x %1.2f\n", Lx, Ly, Lz);
        printf(" Nodes:      %d x %d x %d\n", N, N, N);
        printf(" Time steps: %d\n", K);
        printf(" Time step:  %1.1E\n", dt);
        printf(" Time:       %8.6lf sec\n", t2-t1);
        printf(" Error:      %1.12E\n", residual);

        printf("\n ===================================\n");
    }

    free(xy_buff_prev);
    free(xy_buff_next);
    free(xz_buff_prev);
    free(xz_buff_next);
    free(yz_buff_prev);
    free(yz_buff_next);

    MPI_Finalize();

    return 0;
}

