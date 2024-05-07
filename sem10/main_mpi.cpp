#include <iostream>
#include <cassert>
#include <cstring>
#include <cfloat>
#include <error.h>

#include "mpi.h"
#include "defs.h"
#include "graph.h"

extern int lgsize;
char* inFilename;
char* outFilename;
uint32_t rootNumberToValidate;

void init_sssp(graph_t *G);
void finalize_sssp();
void sssp(vertex_id_t root, graph_t *G, weight_t *distance, uint64_t *traversed_edges);

#include <sys/time.h>
static inline double timer() {
    struct timeval tp;
    struct timezone tzp;

    gettimeofday(&tp, &tzp);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}

void usage(int argc, char **argv)
{
    printf("Usage:\n");
    printf("    %s -in <input> [options]\n", argv[0]);
    printf("Options:\n");
    printf("    -in <input> -- input graph filename\n");
    printf("    -out <output> -- output filename (distances from root vertex). By default output is '<input>.v.<process id>'\n");
    printf("    -root <root> -- root number for validation\n");
    exit(1);
}

void init (int argc, char** argv, graph_t* G)
{
    int i;
    inFilename = outFilename = NULL;
    rootNumberToValidate = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &G->nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &G->rank);


    for ( i = 1; i < argc; ++i) {
   		if (!strcmp(argv[i], "-in")) {
            inFilename = argv[++i];
        }
   		if (!strcmp(argv[i], "-out")) {
            outFilename = argv[++i];
        }
		if (!strcmp(argv[i], "-root")) {
			rootNumberToValidate = (int) atoi(argv[++i]);
        }
    }
    if (!inFilename) usage(argc, argv);
    if (!outFilename) {
        outFilename = (char *)malloc((strlen(inFilename) + 3) * sizeof(char));
        sprintf(outFilename, "%s.v", inFilename);
    }
    //sprintf(outFilename, "%s.%d", outFilename, G->rank);
    sprintf(inFilename, "%s.%d", inFilename, G->rank);
}

void calc_traversed_edges(graph_t *G, weight_t* dist, uint64_t* traversed_edges)
{
    edge_id_t i;
    edge_id_t nedges_local = 0;
    edge_id_t nedges_global = 0;
    for ( i = 0; i < G->local_n_V; i++) {
        if ( dist[i] != -1 ) {
            nedges_local +=  G->rowsIndices[i+1] - G->rowsIndices[i];
        }
    }
    MPI_Allreduce(&nedges_local, &nedges_global, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);
    *traversed_edges = nedges_global;
}

/* write distances from root vertex to each others to output file. -1 = infinity */
void writeDistance(char* filename, weight_t *dist, vertex_id_t n)
{
#ifdef DEBUG
    std::cout << "Distances:";
    for (vertex_id_t i = 0; i < n; i++)
        std::cout << ' ' << dist[i];
    std::cout << std::endl;
#endif
    FILE *F = fopen(filename, "wb");
    assert(fwrite(dist, sizeof(weight_t), n, F) == n);
    fclose(F);
}

void write_validate(graph_t *G, weight_t* local_dist)
{
    weight_t* dist_recv = nullptr;
//    weight_t* dist;

    if (G->rank == 0) dist_recv = (weight_t *)malloc(G->n_V * sizeof(weight_t));

    MPI_Gather(local_dist, G->local_n_V, MPI_DOUBLE, dist_recv, G->local_n_V, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (G->rank == 0) {
//        dist = (weight_t *)malloc(G->n_V * sizeof(weight_t));
//        for (int i = 0; i < G->nproc; i++) {
//            for (vertex_id_t j = 0; j < G->local_n_V; j++) {
//                dist[VERTEX_TO_GLOBAL(i, j)] = dist_recv[G->local_n_V * i + j];
//            }
//        }
        writeDistance(outFilename, dist_recv, G->n_V);
        free(dist_recv);
//        free(dist);
    }
}

int main(int argc, char **argv)
{
    weight_t* local_dist;
    graph_t g;
    uint64_t traversed_edges;
    double* perf;

    /* initializing and reading the graph */
    MPI_Init (&argc, &argv);
    init(argc, argv, &g);

    int err;
    if ((err = g.readGraph(inFilename))) {
        std::cout << "read graph error: " << err << std::endl;
        MPI_Finalize();
        exit(1);
    }

    local_dist = (weight_t *)malloc(g.local_n_V * sizeof(weight_t));

    perf = (double *)malloc(g.nRoots * sizeof(double));

    init_sssp(&g);

    /* doing SSSP */
    for (uint32_t i = 0; i < g.nRoots; ++i) {
        /* initializing for validation, -1 = infinity */
        for (vertex_id_t j = 0; j < g.local_n_V; j++) {
            local_dist[j] = -1;
        }

        //FIXME: timings
        MPI_Barrier(MPI_COMM_WORLD);
        double t1 = timer();
        sssp(g.roots[i], &g, local_dist, &traversed_edges);
        MPI_Barrier(MPI_COMM_WORLD);
        double t2 = timer();

        calc_traversed_edges(&g, local_dist, &traversed_edges);
        g.numTraversedEdges[i] = static_cast<edge_id_t>(traversed_edges);
        perf[i] = g.numTraversedEdges[i] / (1000000 * (t2-t1));

        if (g.rank == 0) {
            printf("%d: numTraversedEdges = %ld, time = %f s, perf = %.4f MTEPS\n", i, g.numTraversedEdges[i], t2-t1, perf[i]);
        }

        if (rootNumberToValidate == i) {
            /* writing for validation */
            write_validate(&g, local_dist);
        }
    }

    finalize_sssp();

    /* final print */
    if (g.rank == 0) {
        double min_perf, max_perf, avg_perf;
        int nNonZeroRoots = 0;
        max_perf = avg_perf = 0;
        min_perf = DBL_MAX;
        for (uint32_t i = 0; i < g.nRoots; ++i) {
            /* we consider search only with large number of traversed edges. Random root can point to connected component with very small number of edges */
            if (g.numTraversedEdges[i] > SMALL_COMPONENT_EDGES_THRESHOLD) {
                ++nNonZeroRoots;
        	    avg_perf += perf[i];
                if (perf[i] < min_perf) min_perf = perf[i];
                if (perf[i] > max_perf) max_perf = perf[i];
            }
        }
        if (nNonZeroRoots != 0) avg_perf /= nNonZeroRoots;
        else error(EXIT_FAILURE, 0, "Number of roots with large traversed edges number is zero");

        printf(
            "%s: nIters = %d SSSP performance min = %.4f avg = %.4f max = %.4f MTEPS\n",
            inFilename, nNonZeroRoots, min_perf, avg_perf, max_perf
        );
    }

    free(local_dist);
    free(perf);
    MPI_Finalize();
    return 0;
}
