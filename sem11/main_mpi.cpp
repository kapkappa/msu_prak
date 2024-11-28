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

    MPI_Allreduce(&max_norm, &max_norm, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

    return max_norm;
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

//    printf("[MPI process %d] I am located at (%d, %d, %d).\n", rank, cart_coords[0], cart_coords[1], cart_coords[2]);

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


    int rank_left = rank - dims[1] * dims[2];
    if (cart_coords[0] == 0)
        rank_left = rank + (dims[0]-1) * dims[1] * dims[2];

    int rank_right = rank + dims[1] * dims[2];
    if (cart_coords[0] == dims[0]-1)
        rank_right = rank - (dims[0]-1) * dims[1] * dims[2];

    int rank_back = rank - dims[2];
    if (cart_coords[1] == 0)
        rank_back = rank + (dims[1]-1) * dims[2];

    int rank_front = rank + dims[2];
    if (cart_coords[1] == dims[1]-1)
        rank_front = rank - (dims[1]-1) * dims[2];

    int rank_below = rank - 1;
    if (cart_coords[2] == 0)
        rank_below = rank + (dims[2] - 1);

    int rank_above = rank + 1;
    if (cart_coords[2] == dims[2]-1)
        rank_above = rank - (dims[2] - 1);


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

        // X dim, YZ plain
        // send PREV to RIGHT and receive PREV from LEFT
        MPI_Sendrecv(U_curr.yz(x_size-1), y_size*z_size, MPI_DOUBLE, rank_right, 0,
                     yz_buff_prev,        y_size*z_size, MPI_DOUBLE, rank_left , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // send NEXT to LEFT and receive NEXT from RIGHT
        MPI_Sendrecv(U_curr.yz(0), y_size*z_size, MPI_DOUBLE, rank_left , 0,
                     yz_buff_next, y_size*z_size, MPI_DOUBLE, rank_right, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        //Y dim, XZ plain
        // send PREV to FRONT and recieve PREV from BACK
        MPI_Sendrecv(U_curr.xz(y_prev_offset), 1,             XZ,         rank_front, 0,
                     xz_buff_prev,             x_size*z_size, MPI_DOUBLE, rank_back , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // send NEXT to BACK and receive NEXT from FRONT
        MPI_Sendrecv(U_curr.xz(y_next_offset), 1,             XZ,         rank_back,  0,
                     xz_buff_next,             x_size*z_size, MPI_DOUBLE, rank_front, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


        //Z dim, XY plain
        // send PREV to ABOVE and receive from BELOW
        MPI_Sendrecv(U_curr.xy(z_prev_offset), 1,             XY,         rank_above, 0,
                     xy_buff_prev,             x_size*y_size, MPI_DOUBLE, rank_below, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // send NEXT to BELOW and receive from ABOVE
        MPI_Sendrecv(U_curr.xy(z_next_offset), 1,             XY,         rank_below, 0,
                     xy_buff_next,             x_size*y_size, MPI_DOUBLE, rank_above, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

#pragma omp parallel for
        for (int i = from; i < to; i++) {
            for (int j = 0; j < y_size; j++) {
                for (int k = 0; k < z_size; k++) {
                    double x_diff, y_diff, z_diff;

                    if (i == 0) {
                        x_diff = yz_buff_prev[j*z_size + k] - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                    } else if (i == x_size-1) {
                        x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + yz_buff_next[j*z_size + k];
                    } else {
                        x_diff = U_curr(i-1,j,k) - 2 * U_curr(i,j,k) + U_curr(i+1,j,k);
                    }

                    if (j == 0) {
                        y_diff = xz_buff_prev[i*z_size+k] - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    } else if (j == y_size-1) {
                        y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + xz_buff_next[i*z_size+k];
                    } else {
                        y_diff = U_curr(i,j-1,k) - 2 * U_curr(i,j,k) + U_curr(i,j+1,k);
                    }

                    if (k == 0) {
                        z_diff = xy_buff_prev[i*y_size+j] - 2 * U_curr(i,j,k) + U_curr(i,j,k+1);
                    } else if (k == z_size-1) {
                        z_diff = U_curr(i,j,k-1) - 2 * U_curr(i,j,k) + xy_buff_next[i*y_size+j];
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

