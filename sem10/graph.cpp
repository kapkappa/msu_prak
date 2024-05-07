#include "graph.h"

#include <cstring>
#include <cmath>
#include <error.h>
#include <iostream>

int lgsize;

/* graph representation:
    n_V
    arity
    local_n_V
    local_n_E
    directed
    align
    rowIndices
    endV
    nRoots
    roots
    numTraversedEdges
    weights
*/
int graph_t::readGraph(char *filename) {
    #ifdef DEBUG
        std::cout << __func__ << " filename: " << filename << std::endl;
    #endif
    edge_id_t arity;
    uint8_t align;

    FILE *F = fopen(filename, "rb");
    if (!F) error(EXIT_FAILURE, 0, "Error in opening file %s", filename);

	if (fread(&n_V, sizeof(vertex_id_t), 1, F) != 1) return 1;
    if (fread(&arity, sizeof(edge_id_t), 1, F) != 1) return 2;

    n_E = n_V * arity;

    if (fread(&local_n_V, sizeof(vertex_id_t), 1, F) != 1) return 3;
    if (fread(&local_n_E, sizeof(edge_id_t), 1, F) != 1) return 4;

    /* Check for power of 2 */
    if ((nproc & (nproc - 1)) != 0) {
        if (rank == nproc -1)
            printf("Number of processes must be a power of two\n");
        return 5;
    }
    lgsize = log(nproc)/log(2);
    if (lgsize >= nproc) return 6;

    if (fread(&directed, sizeof(bool), 1, F) != 1) return 7;
    if (fread(&align, sizeof(uint8_t), 1, F) != 1) return 8;

	rowsIndices = (edge_id_t *)malloc((local_n_V + 1) * sizeof(edge_id_t));
    if (!rowsIndices) return 9;

	if (fread(rowsIndices, sizeof(edge_id_t), local_n_V + 1, F) != local_n_V + 1) return 10;

    endV = (vertex_id_t *)malloc(rowsIndices[local_n_V] * sizeof(vertex_id_t));
    if (!endV) return 11;

    if (fread(endV, sizeof(vertex_id_t), rowsIndices[local_n_V], F) != rowsIndices[local_n_V]) return 12;
    if (fread(&nRoots, sizeof(uint32_t), 1, F) != 1) return 13;

    roots = (vertex_id_t *)malloc(nRoots * sizeof(vertex_id_t));
    if (!roots) return 14;

    numTraversedEdges = (edge_id_t *)malloc(nRoots * sizeof(edge_id_t));
    if (!numTraversedEdges) return 15;

    if (fread(roots, sizeof(vertex_id_t), nRoots, F) != nRoots) return 16;
    if (fread(numTraversedEdges, sizeof(edge_id_t), nRoots, F) != nRoots) return 17;

    weights = (weight_t *)malloc(local_n_E * sizeof(weight_t));
    if (!weights) return 18;

    if (fread(weights, sizeof(weight_t), local_n_E, F) != local_n_E) return 19;

    fclose(F);
    return 0;
}

int graph_t::parallelReadGraph(char *filename, int nfiles) {
    #ifdef DEBUG
        std::cout << __func__ << " filename: " << filename << std::endl;
    #endif
    edge_id_t arity;
    uint8_t align;

    FILE* F[nfiles];
    vertex_id_t tmp_local_n_V[nfiles];
    edge_id_t tmp_local_n_E[nfiles];

    char*full_filename = (char*) malloc((strlen(filename) + 3) * sizeof(char));
    for (int i = 0; i < nfiles; i++) {
        sprintf(full_filename, "%s.%d", filename, i);
        F[i] = fopen(full_filename, "rb");
        if (!F[i]) error(EXIT_FAILURE, 0, "Error in opening file %s.%d", filename, i);
    }

    for (int i = 0; i < nfiles; i++) {
    	if (fread(&n_V, sizeof(vertex_id_t), 1, F[i]) != 1) return 1;
        if (fread(&arity, sizeof(edge_id_t), 1, F[i]) != 1) return 2;
        if (fread(&tmp_local_n_V[i], sizeof(vertex_id_t), 1, F[i]) != 1) return 3;
        if (fread(&tmp_local_n_E[i], sizeof(edge_id_t), 1, F[i]) != 1) return 4;
    }

    n_E = n_V * arity;
    local_n_E = n_E;
    local_n_V = n_V;

    /* Check for power of 2 */
    if ((nproc & (nproc - 1)) != 0) {
        if (rank == nproc -1)
            printf("Number of processes must be a power of two\n");
        return 5;
    }
    lgsize = log(nproc)/log(2);
    if (lgsize >= nproc) return 6;

    for (int i = 0; i < nfiles; i++) {
        if (fread(&directed, sizeof(bool), 1, F[i]) != 1) return 7;
        if (fread(&align, sizeof(uint8_t), 1, F[i]) != 1) return 8;
    }

	rowsIndices = (edge_id_t *)malloc((n_V + 1) * sizeof(edge_id_t));
    if (!rowsIndices) return 9;

	edge_id_t* local_rowsIndices = (edge_id_t *)malloc((n_V + 1) * sizeof(edge_id_t));
    if (!local_rowsIndices) return 9;

    int offset = 0;
    rowsIndices[0] = 0;
    for (int i = 0; i < nfiles; i++) {
    	if (fread(local_rowsIndices, sizeof(edge_id_t), tmp_local_n_V[i] + 1, F[i]) != tmp_local_n_V[i] + 1) return 10;

        for (vertex_id_t j = 0; j < tmp_local_n_V[i]; j++)
            rowsIndices[offset + j + 1] = rowsIndices[offset + j] + (local_rowsIndices[j + 1] - local_rowsIndices[j]);

        offset += tmp_local_n_V[i];
    }
    free(local_rowsIndices);

    endV = (vertex_id_t *)malloc(2*n_E * sizeof(vertex_id_t));
    if (!endV) return 11;

    offset = 0;
    for (int i = 0; i < nfiles; i++) {
        if (fread(endV+offset, sizeof(vertex_id_t), tmp_local_n_E[i], F[i]) != tmp_local_n_E[i]) return 12;
        offset += tmp_local_n_E[i];
        if (fread(&nRoots, sizeof(uint32_t), 1, F[i]) != 1) return 13;
    }

    roots = (vertex_id_t *)malloc(nRoots * sizeof(vertex_id_t));
    if (!roots) return 14;

    numTraversedEdges = (edge_id_t *)malloc(nRoots * sizeof(edge_id_t));
    if (!numTraversedEdges) return 15;

    for (int i = 0; i < nfiles; i++) {
        if (fread(roots, sizeof(vertex_id_t), nRoots, F[i]) != nRoots) return 16;
        if (fread(numTraversedEdges, sizeof(edge_id_t), nRoots, F[i]) != nRoots) return 17;
    }

    weights = (weight_t *)malloc(n_E * sizeof(weight_t));
    if (!weights) return 18;

    offset = 0;
    for (int i = 0; i < nfiles; i++) {
        if (fread(weights+offset, sizeof(weight_t), tmp_local_n_E[i], F[i]) != tmp_local_n_E[i]) return 19;
        offset += tmp_local_n_E[i];
    }

    for (int i = 0; i < nfiles; i++)
        fclose(F[i]);
    return 0;
}

int graph_t::writeGraph(char* filename) {
    FILE *F = fopen(filename, "wb");
    if (!F) error(EXIT_FAILURE, 0, "Error in opening file %s", filename);

    if (fwrite(&n_V, sizeof(vertex_id_t), 1, F) != 1) return 1;

    edge_id_t arity = n_E / n_V;

    if (fwrite(&arity, sizeof(edge_id_t), 1, F) != 1) return 2;

    if (fwrite(&local_n_V, sizeof(vertex_id_t), 1, F) != 1) return 3;
    if (fwrite(&local_n_E, sizeof(edge_id_t), 1, F) != 1) return 4;
    if (fwrite(&directed, sizeof(bool), 1, F) != 1) return 5;
    uint8_t align = 0;
    if (fwrite(&align, sizeof(uint8_t), 1, F) != 1) return 6;
    if (fwrite(rowsIndices, sizeof(edge_id_t), local_n_V+1, F) != local_n_V+1) return 7;
    if (fwrite(endV, sizeof(vertex_id_t), rowsIndices[local_n_V], F) != rowsIndices[local_n_V]) return 8;
    if (fwrite(&nRoots, sizeof(uint32_t), 1, F) != 1) return 9;
    if (fwrite(roots, sizeof(vertex_id_t), nRoots, F) != nRoots) return 10;
    if (fwrite(numTraversedEdges, sizeof(edge_id_t), nRoots, F) != nRoots) return 11;
    if (fwrite(weights, sizeof(weight_t), local_n_E, F) != local_n_E) return 12;

    fclose(F);
    return 0;
}

void graph_t::printGraph() {
#ifdef DEBUG
    std::cout << "nV: " << n_V << " arity: " << arity << " local nE: " << local_n_V << " local_n_E: " << local_n_E << std::endl;
#endif
    for (vertex_id_t i = 0; i < local_n_V; ++i) {
        printf("%-3d:", i);
        for (edge_id_t j = rowsIndices[i]; j < rowsIndices[i+1]; ++j)
            printf("%d (%f), ", endV[j], weights[j]);
        printf("\n");
    }
}
