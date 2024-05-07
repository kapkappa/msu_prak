#pragma once

#include <cstdint>

#define DEFAULT_ARITY 16
#define SMALL_COMPONENT_EDGES_THRESHOLD   2
#define FNAME_LEN   256

#define START_BUF_LEN 128

typedef uint32_t vertex_id_t;
typedef uint64_t edge_id_t;
typedef double weight_t;

#define MAX_NLEVELS 500 // max number of levels in bfs for validation

extern int lgsize; /* log2 (number of processes) */
#define MOD_SIZE(v) ((v) & ((1 << lgsize) - 1))
#define DIV_SIZE(v) ((v) >> lgsize)
#define MUL_SIZE(x) ((x) << lgsize)

/* macroses for obtaining vertices distribution between nodes */
#define VERTEX_OWNER(v) ((int)(MOD_SIZE(v)))
#define VERTEX_LOCAL(v) ((vertex_id_t)(DIV_SIZE(v)))
#define VERTEX_TO_GLOBAL(i, r) ((vertex_id_t)(MUL_SIZE((vertex_id_t)i) + (int)(r)))
