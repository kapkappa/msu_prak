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
//    if (rank == 0) columns_number += disps;

    double ** columns = (double **)malloc(columns_number * sizeof(double *));

    dense_matrix A(size, size);

    std::vector<double> b;

    if (rank == 0) {
        A.generate();
        b = generate_vector(A, size);
        A.transpose();
        for (uint32_t i = 0; i < columns_number; i++)
            columns[i] = A.val + i * world_size * size;
    } else {
        for (uint32_t i = 0; i < columns_number; i++)
            columns[i] = (double *)malloc(size * sizeof(double));
    }

    for (uint32_t i = 0; i < columns_number; i++)
        MPI_Scatter(A.val + i * world_size * size, size, MPI_DOUBLE, columns[i], size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double * hh = (double*)malloc(size * sizeof(double));

    uint32_t nrows = A.nrows, ncols = A.ncols;

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

        if (rank == 0) {
            double sum = 0.0;
            for (uint32_t j = 0; j < len; j++)
                sum += 2.0 * hh[j] * b[j+i];
            for (uint32_t j = 0; j < len; j++)
                b[j+i] -= sum * hh[j];
        }
    }

    double t1 = timer();
/*
    for (uint32_t i = 0; i < size; i++) {
        if (rank == i % world_size) {
            for (uint32_t j = 0; j < size; j++)
                std::cout << columns[i / world_size][j] << ' ';
            std::cout << std::endl;
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    for (uint32_t i = 0; i < columns_number; i++)
        MPI_Gather(columns[i], size, MPI_DOUBLE, A.val + i * world_size * size, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

if (rank == 0) {
//    A.transpose();
    A.print();
    print(b);
}
*/
    double * x = (double *)calloc(size, sizeof(double));
    double * x_global = (double *)calloc(size, sizeof(double));

    for (int32_t i = size-1; i >= 0; i--) {
        double sum = 0.0, mul = 0.0;

        for (uint32_t j = i+1; j < size; j++) {
            if (rank == i % world_size) {
                if (rank == j % world_size) {
                    sum += x[j] * columns[j / world_size][i];
                } else {
                    MPI_Recv(&mul, 1, MPI_DOUBLE, j % world_size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    sum += mul;
                }
            } else {
                if (rank == j % world_size) {
                    mul = x[j] * columns[j / world_size][i];
                    MPI_Send(&mul, 1, MPI_DOUBLE, i % world_size, 0, MPI_COMM_WORLD);
                }
            }

        }

        if (rank != i % world_size && rank == 0) {
            MPI_Send(&b[i], 1, MPI_DOUBLE, i % world_size, 0, MPI_COMM_WORLD);
        } else if (rank == i % world_size) {
            if (i % world_size != 0)
                MPI_Recv(&mul, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            else
                mul = b[i];
            sum = mul - sum;
            x[i] = sum / columns[i / world_size][i];
        }
    }

    MPI_Allreduce(x, x_global, size, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

/*
    for (uint32_t t_rank = 0; t_rank < world_size; t_rank++) {
        if (rank == t_rank) {
            std::cout << "x local: ";
            for (uint32_t i = 0; i < size; i++)
                std::cout << x[i] << ' ';
            std::cout << std::endl << "x global: ";
            for (uint32_t i = 0; i < size; i++)
                std::cout << x_global[i] << ' ';
            std::cout << std::endl;
        }
    }
*/

    double t2 = timer();

    std::cout << "Hosehold time: " << t1-t0 << std::endl;
    std::cout << "Gauss time: " << t2-t1 << std::endl;
    std::cout << "Total time: " << t2-t0 << std::endl;
    std::cout << "Error norm: " << get_error_norm(x_global, size) << std::endl;

    free(hh);
    free(x);
    free(x_global);

    if (rank != 0) {
        for (uint32_t i = 0; i < columns_number; i++)
            free(columns[i]);
    }

    free(columns);

    MPI_Finalize();

    return 0;
}
