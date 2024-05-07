#include <error.h>
#include <time.h>
#include <cassert>
#include <cstring>
#include <cfloat>
#include <iostream>

#include "defs.h"
#include "graph.h"

char* inFilename;
char* outFilename;
uint32_t rootNumberToValidate;
uint32_t nIters;
int nFiles;

#if defined(CLOCK_MONOTONIC)
#define CLOCK CLOCK_MONOTONIC
#elif defined(CLOCK_REALTIME)
#define CLOCK CLOCK_REALTIME
#else
#error "Failed to find a timing clock."
#endif

void init_sssp(graph_t *G);
void finalize_sssp();
void sssp(vertex_id_t root, graph_t *G, weight_t *dist, uint64_t *traversed_edges);

void usage(int argc, char **argv)
{
    printf("Usage:\n");
    printf("    %s -in <input> [options]\n", argv[0]);
    printf("Options:\n");
    printf("    -in <input> -- input graph filename\n");
    printf("    -out <output> -- output filename. Default output is '<input>.v'\n");
    printf("    -root <root> -- root number for validation. Default is the first vertex\n");
    printf("    -nIters <nIters> -- number of iterations\n");
    printf("    -nFiles <nFiles> -- number of files for parallel reading\n");
    exit(1);
}

void init (int argc, char** argv)
{
    inFilename = outFilename = NULL;
    nIters = 1;
    nFiles = 1;
    rootNumberToValidate = 0;
    for (int i = 1; i < argc; ++i) {
   		if (!strcmp(argv[i], "-in")) {
            inFilename = argv[++i];
        }
   		if (!strcmp(argv[i], "-out")) {
            outFilename = argv[++i];
        }
		if (!strcmp(argv[i], "-root")) {
			rootNumberToValidate = (int) atoi(argv[++i]);
        }
		if (!strcmp(argv[i], "-nIters")) {
			nIters = (int) atoi(argv[++i]);
        }
		if (!strcmp(argv[i], "-nFiles")) {
			nFiles = (int) atoi(argv[++i]);
        }
    }
    if (!inFilename) usage(argc, argv);
    if (!outFilename) {
        outFilename = (char *)malloc((strlen(inFilename) + 3) * sizeof(char));
        sprintf(outFilename, "%s.v", inFilename);
    }
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

/* write number of vertices at each level */
void writeLevels (char* filename, vertex_id_t *validateNLevels, int validateNLevelsLength)
{
    FILE *F = fopen(filename, "w"); 
    fprintf(F, "%d\n", validateNLevelsLength);
    for (int i = 0; i < validateNLevelsLength; ++i) 
        fprintf(F, "%d\n", validateNLevels[i]);
}

int main(int argc, char **argv)
{
    weight_t* dist = NULL;
    graph_t g;
    struct timespec start_ts, finish_ts;
    double *perf;
    int err;

    /* initializing and reading the graph */
    init(argc, argv);

    if (nFiles > 1) {
        if ((err = g.parallelReadGraph(inFilename, nFiles))) {
            std::cout << "parallelReadGraph returned error " << err << std::endl;
            exit(1);
        }
    } else {
        if ((err = g.readGraph(inFilename))) {
            std::cout << "readGraph returned error " << err << std::endl;
            exit(1);
        }
    }

//    g.printGraph();

    init_sssp(&g);
    dist = (weight_t *)malloc(g.n_V * sizeof(weight_t));

    perf = (double *)malloc(g.nRoots * sizeof(double));

    nIters = (nIters < g.nRoots) ? nIters : g.nRoots;

    printf("start algorithm iterations...\n");
    for (uint32_t i = 0; i < nIters; ++i) {
        /* initializing, -1 == infinity */
        for (vertex_id_t i = 0; i < g.n_V; i++) {
            dist[i] = -1;
        }

        uint64_t nedges;
        clock_gettime(CLOCK, &start_ts);
        sssp(g.roots[i], &g, dist, &nedges);
        clock_gettime(CLOCK, &finish_ts);
        double time =
            (finish_ts.tv_nsec - (double)start_ts.tv_nsec) * 1.0e-9 +
            (finish_ts.tv_sec - (double)start_ts.tv_sec);

        g.numTraversedEdges[i] = static_cast<edge_id_t>(nedges);
        perf[i] = g.numTraversedEdges[i] / (1000000 * time);
        printf("%d: numTraversedEdges = %ld, time = %f s, perf = %.4f MTEPS\n", i, g.numTraversedEdges[i], time, perf[i] );

        if (rootNumberToValidate == i) {
            /* writing for validation */
            writeDistance(outFilename, dist, g.n_V);
        }
    }
    printf("algorithm iterations finished.\n");

    /* final print */
    double min_perf, max_perf, avg_perf;
    int nNonZeroRoots = 0;
    max_perf = avg_perf = 0;
    min_perf = DBL_MAX;
    for (uint32_t i = 0; i < nIters; ++i) {
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

    finalize_sssp();

    return 0;
}
