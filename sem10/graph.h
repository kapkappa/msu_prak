#pragma once

#include "defs.h"

#include <cstdlib>

/***
 The minimal graph repesentation consists of:
 n_V        -- the number of vertices
 n_E        -- the number of edges
 endV     -- an array of size 2*m (m for directed graphs) that stores the
             destination ID of an edge <src->dest>.
 rowsIndices -- an array of size n+1 that stores the degree
             (out-degree in case of directed graphs) and pointers to
             the endV array. The degree of vertex i is given by
             rowsIndices[i+1]-rowsIndices[i], and the edges out of i are
             stored in the contiguous block endV[rowsIndices[i] .. rowsIndices[i+1]-1].
 Vertices are ordered from 0 in our internal representation
 ***/

//extern int lgsize;

class graph_t {
private:
    int lgsize;
public:
    int scale; /* log2 of vertices number */
    vertex_id_t n_V;
    edge_id_t n_E;
    int avg_vertex_degree; /* relation m / n */ /* generation only */
    bool directed;

    double a, b, c; /* RMAT graph parameters*/ /* generation only */

    bool permute_vertices; /* generation only */

    edge_id_t* rowsIndices;
    vertex_id_t* endV;

    /* Edge weights */
    weight_t* weights;

    weight_t min_weight, max_weight; /* generation only */

    /* Search parameters */
    uint32_t nRoots;
    vertex_id_t *roots;
    edge_id_t *numTraversedEdges;

    /* Distributed version parameters */
    int nproc, rank;
    vertex_id_t local_n_V; /* local vertices number */
    edge_id_t local_n_E; /* local edges number */

    graph_t() {
        nproc = 1;
        rank = 0;
    }

    ~graph_t() {
        if (rowsIndices) free(rowsIndices);
        if (endV) free(endV);
        if (roots) free(roots);
        if (weights) free(weights);
        if (numTraversedEdges) free(numTraversedEdges);
    }

    /* read graph from file */
    int readGraph(char *filename);
    int writeGraph(char *filename);
    void printGraph();
};
