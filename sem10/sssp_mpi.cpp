#include <iostream>
#include <vector>
#include <set>
#include <cmath>

#include "graph.h"
#include "defs.h"
#include "mpi.h"

std::vector<std::set<vertex_id_t>*> buckets;

std::vector<int> send_counts;
std::vector<int> recv_counts;

struct message {
    weight_t weight;
    vertex_id_t destination;
};

MPI_Datatype message_type;

std::vector<std::vector<message>> send_buffers;
std::vector<std::vector<message>> recv_buffers;

void *buffer_attached;
int buffer_length;
int buffer_attached_size;

void init_sssp(graph_t *G)
{
    struct message msg = {0, 0};

    int nblocks = 2, blocklen[2] = {1, 1};
    MPI_Aint displ[2];
    MPI_Aint base_address;
    MPI_Get_address(&msg, &base_address);
    MPI_Get_address(&msg.weight, &displ[0]);
    MPI_Get_address(&msg.destination, &displ[1]);
    displ[0] = MPI_Aint_diff(displ[0], base_address);
    displ[1] = MPI_Aint_diff(displ[1], base_address);

    MPI_Datatype old_types[2] = {MPI_DOUBLE, MPI_UINT32_T};
    MPI_Type_create_struct(nblocks, blocklen, displ, old_types, &message_type);
    MPI_Type_commit(&message_type);

    buffer_length = START_BUF_LEN;
    buffer_attached_size = (MPI_BSEND_OVERHEAD + sizeof(message_type)) * buffer_length;
    buffer_attached = malloc(buffer_attached_size);
    MPI_Buffer_attach(buffer_attached, buffer_attached_size);
}

void finalize_sssp()
{
    MPI_Type_free(&message_type);
    MPI_Buffer_detach(&buffer_attached, &buffer_attached_size);
    if (buffer_attached) free(buffer_attached);
}


void relax(graph_t* G, weight_t* distance, vertex_id_t v, weight_t x, weight_t delta) {
#ifdef DEBUG
    std::cout << __func__ << " with params: " << v << ' ' << x << ' ' << delta << " vertex owner: " << v / G->local_n_V << " vertex local: " << v % G->local_n_V << std::endl;
#endif

    int vertex_owner = v / G->local_n_V;
    int vertex_local = v % G->local_n_V;

    if (vertex_owner == G->rank) {
        v = vertex_local;
//    if (VERTEX_OWNER(v) == G->rank) {
        if (x < distance[v] || distance[v] < 0) {
            size_t new_index = floor(x / delta);

            if (new_index >= buckets.size())
                buckets.resize(new_index+1, nullptr);

            if (buckets[new_index] == nullptr)
                buckets[new_index] = new std::set<vertex_id_t>;

            if (distance[v] != -1) {
                size_t old_index = floor(distance[v] / delta);
                if (buckets[old_index] != nullptr)
                    buckets[old_index]->erase(v);
            }
            buckets[new_index]->insert(v);

            distance[v] = x;
        }
    } else {
        send_buffers[vertex_owner].push_back( {x, v} );
        send_counts[vertex_owner]++;
    }
}

void synchronize(graph_t* G, weight_t* distance, double delta) {
#ifdef DEBUG
    std::cout << __func__ << std::endl;
#endif

    MPI_Alltoall(send_counts.data(), 1, MPI_INT, recv_counts.data(), 1, MPI_INT, MPI_COMM_WORLD);

    int total_cnt = 0;
    for (const auto& cnt : send_counts) total_cnt += cnt;

    if (total_cnt > buffer_length) {
        MPI_Buffer_detach(&buffer_attached, &buffer_attached_size);
        do {
            buffer_length *= 2;
        } while (buffer_length < total_cnt);
        buffer_attached_size = (MPI_BSEND_OVERHEAD + sizeof(message_type)) * buffer_length;
        buffer_attached = realloc(buffer_attached, buffer_attached_size);
        MPI_Buffer_attach(buffer_attached, buffer_attached_size);
    }

    for (int i = 0; i < G->nproc; i++) {
        if (send_counts[i] && i != G->rank) {
#ifdef DEBUG
            std::cout << "send " << send_counts[i] << " from " << G->rank << " to " << i << std::endl;
#endif
            MPI_Bsend(send_buffers[i].data(), send_counts[i], message_type, i, 0, MPI_COMM_WORLD);
        }
    }

    for (int i = 0; i < G->nproc; i++) {
        if (recv_counts[i] && i != G->rank) {
#ifdef DEBUG
            std::cout << "recv " << recv_counts[i] << " from " << i << std::endl;
#endif
            recv_buffers[i].resize(recv_counts[i]);
            MPI_Recv(recv_buffers[i].data(), recv_counts[i], message_type, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    for (int i = 0; i < G->nproc; i++) {
        for (const auto& msg : recv_buffers[i]) {
            relax(G, distance, msg.destination, msg.weight, delta);
        }
    }

    for (int i = 0; i < G->nproc; i++) {
        recv_buffers[i].clear();
        send_buffers[i].clear();
        send_counts[i] = 0;
        recv_counts[i] = 0;
    }

    MPI_Barrier(MPI_COMM_WORLD);
//    MPI_Buffer_detach(&buffer_attached, &buffer_attached_size);
//    free(buffer_attached);
}

void sssp(vertex_id_t root, graph_t *G, weight_t *distance, uint64_t *traversed_edges)
{
    double delta = 10.0 * G->n_V / G->n_E;

    if (G->rank == 0)
        std::cout << "Start sssp with delta=" << delta << std::endl;

    uint64_t nedges = 0;

    send_counts.clear();
    send_counts.resize(G->nproc, 0);

    recv_counts.clear();
    recv_counts.resize(G->nproc, 0);

    send_buffers.clear();
    send_buffers.resize(G->nproc);

    recv_buffers.clear();
    recv_buffers.resize(G->nproc);

    if ((int)(root / G->local_n_V) == G->rank) {
//    if (VERTEX_OWNER(root) == G->rank) {
        relax(G, distance, root, 0, delta);
    }

//    MPI_Barrier();

    uint64_t current_bucket = 0;
    const uint64_t max_bucket = std::numeric_limits<uint64_t>::max();

    // main cycle
    while (true) {
//        MPI_Barrier(MPI_COMM_WORLD);
        synchronize(G, distance, delta);

        // get minimum bucket
        while (current_bucket < buckets.size() && (buckets[current_bucket] == nullptr || buckets[current_bucket]->empty()))
            current_bucket++;
        if (current_bucket >= buckets.size())
            current_bucket = max_bucket;

        MPI_Allreduce(&current_bucket, &current_bucket, 1, MPI_UINT64_T, MPI_MIN, MPI_COMM_WORLD);

        if (current_bucket == max_bucket) break;

        std::set<vertex_id_t> deleted_vertices;

        int num_buckets = 1;
//        std::cout << "light phase" << std::endl;
        while (num_buckets > 0) {

            if (current_bucket < buckets.size() && buckets[current_bucket] != nullptr) {
                std::set<vertex_id_t>& bucket = *buckets[current_bucket];

                while (!bucket.empty()) {
                    vertex_id_t u = *bucket.begin();
                    bucket.erase(bucket.begin());

                    deleted_vertices.insert(u);

                    weight_t dist = distance[u];
                    for (auto j = G->rowsIndices[u]; j < G->rowsIndices[u+1]; j++) {
                        if (G->weights[j] <= delta)
                            relax(G, distance, G->endV[j], dist + G->weights[j], delta);
                        nedges++;
                    }
                }
            }

//            MPI_Barrier(MPI_COMM_WORLD);
            synchronize(G, distance, delta);

            num_buckets = (current_bucket < buckets.size() && buckets[current_bucket] != nullptr && !buckets[current_bucket]->empty());
            MPI_Allreduce(&num_buckets, &num_buckets, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        }

//        std::cout << "heavy phase" << std::endl;
        for (const auto& u : deleted_vertices) {
            weight_t dist = distance[u];
            for (auto j = G->rowsIndices[u]; j < G->rowsIndices[u+1]; j++) {
                if (G->weights[j] > delta)
                    relax(G, distance, G->endV[j], dist + G->weights[j], delta);
                nedges++;
            }
        }

        current_bucket++;
    }

    for (std::set<vertex_id_t>* b : buckets) {
        if (b) free(b);
    }

    buckets.clear();

    *traversed_edges = nedges;
}
