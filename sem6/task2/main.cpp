#include "dense_matrix.h"
#include "operations.h"

#include <mpi.h>

#include <sys/time.h>
#include <iostream>
#include <cmath>

int rank, world_size;

static inline double timer() {
    struct timeval tp;
    struct timezone tzp;

    gettimeofday(&tp, &tzp);
//    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
    return MPI_Wtime();
}

static inline double sgn(double x) {
    if (x > 0)
        return 1.0;
    if (x < 0)
        return -1.0;
    return 0.0;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::cout << "World size: " << world_size << std::endl;

    uint32_t size;
    if (argc == 1)
        size = 10;
    else
        size = atoi(argv[1]);

    uint32_t columns_number = size / world_size, disps = size % world_size;
    for (uint32_t i = 0; i < disps; i++) {
        if (rank == i) {
            columns_number++;
        }
    }

    double ** columns = (double **)malloc(columns_number * sizeof(double *));
    double * x = (double *)calloc(size, sizeof(double));
    double * x_global = (double *)calloc(size, sizeof(double));
    double * b = (double *)calloc(size, sizeof(double));
    double * b_local = nullptr;
    if (rank == 0) b_local = (double *)calloc(size, sizeof(double));

    dense_matrix A(size, size);

    if (rank == 0) {
        A.generate();
        generate_vector(A, b, size);
        A.transpose();
        for (uint32_t i = 0; i < columns_number; i++)
            columns[i] = A.val + i * world_size * size;
    } else {
        for (uint32_t i = 0; i < columns_number; i++)
            columns[i] = (double *)malloc(size * sizeof(double));
    }

    for (uint32_t i = 0; i < size / world_size; i++)
        MPI_Scatter(A.val + i * world_size * size, size, MPI_DOUBLE, columns[i], size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (uint32_t i = 1; i < disps; i++) {
        if (rank == 0)
            MPI_Send(A.val + size * world_size * (size / world_size) + i * size, size, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        else if (rank == i)
            MPI_Recv(columns[columns_number-1], size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if (size % world_size != 0) {
        if (rank == 0)
            MPI_Send(b, size, MPI_DOUBLE, size % world_size, 0, MPI_COMM_WORLD);
        else if (rank == size % world_size)
            MPI_Recv(b, size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    double * hh = (double*)malloc(size * sizeof(double));

    uint32_t nrows = A.nrows, ncols = A.ncols;

    MPI_Barrier(MPI_COMM_WORLD);

    double t0 = timer();

    for (uint32_t i = 0; i < size-1; i++) {
/////// CREATING HAUSEHOLDER VECTOR X
        uint32_t len = nrows - i;
        if (rank == i % world_size) {
            for (uint32_t j = i; j < nrows; j++)
                hh[j-i] = columns[i / world_size][j];

            double hh_norm = get_norm(hh, len);
            hh[0] += sgn(hh[0]) * hh_norm;
            hh_norm = get_norm(hh, len);

            for (uint32_t j = 0; j < len; j++)
                hh[j] /= hh_norm;
        }

        MPI_Bcast(hh, len, MPI_DOUBLE, i % world_size, MPI_COMM_WORLD);

//////  HAUSEHOLDER DECOMPOSITION
        for (uint32_t col = i / world_size; col < columns_number; col++) {
            if (col * world_size + rank >= i) {
                double sum = 0.0;
                for (uint32_t k = 0; k < len; k++)
                    sum += 2.0 * hh[k] * columns[col][k+i];
                for (uint32_t k = 0; k < len; k++)
                    columns[col][k+i] -= sum * hh[k];
            }
        }

        if (rank == size % world_size) {
            double sum = 0.0;
            for (uint32_t j = 0; j < len; j++)
                sum += 2.0 * hh[j] * b[j+i];
            for (uint32_t j = 0; j < len; j++)
                b[j+i] -= sum * hh[j];
        }
    }

    double t1 = timer();

    if (size % world_size != 0) {
        if (rank == size % world_size) {
            MPI_Send(b, size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        } else if (rank == 0) {
            MPI_Recv(b_local, size, MPI_DOUBLE, size % world_size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else if (rank == 0) {
        for (uint32_t i = 0; i < size; i++)
            b_local[i] = b[i];
    }

    for (int32_t i = size - 1; i >= 0; i--) {
        if (rank == i % world_size) {
            if (world_size > 1) MPI_Recv(b, size, MPI_DOUBLE, (i+1) % world_size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            x[i] = b[i] / columns[i / world_size][i];
            for (uint32_t j = 0; j <= i; j++)
                b[j] -= x[i] * columns[i / world_size][j];

        } else if (rank == (i+1) % world_size) {
            if (world_size > 1) MPI_Send(b, size, MPI_DOUBLE, i % world_size, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Allreduce(x, x_global, size, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    double t2 = timer();

    double local_hh_time = t1-t0, hh_time;
    double local_gs_time = t2-t1, gs_time;
    double local_all_time = t2-t0, all_time;

    MPI_Allreduce(&local_hh_time, &hh_time, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    MPI_Allreduce(&local_gs_time, &gs_time, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    MPI_Allreduce(&local_all_time, &all_time, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

    for (uint32_t i = 0; i < size / world_size; i++)
        MPI_Gather(columns[i], size, MPI_DOUBLE, A.val + i * world_size * size, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (uint32_t i = 1; i < disps; i++) {
        if (rank == i)
            MPI_Send(columns[columns_number-1], size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        else if (rank == 0)
            MPI_Recv(A.val + world_size * size * (size / world_size) + i * size, size, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if (rank == 0) {
        A.transpose();

        std::cout << "Hosehold time: " << hh_time << std::endl;
        std::cout << "Gauss time: " << gs_time << std::endl;
        std::cout << "Total time: " << all_time << std::endl;
        std::cout << "||Ax-b|| = " << get_discrepancy(A, x_global, b_local) << std::endl;
        std::cout << "Error norm: " << get_error_norm(x_global, size) << std::endl;
    }

    free(hh);
    free(x);
    free(x_global);

    if (rank != 0) {
        for (uint32_t i = 0; i < columns_number; i++)
            free(columns[i]);
    } else {
        free(b_local);
    }

    free(columns);

    free(b);

    MPI_Finalize();

    return 0;
}
