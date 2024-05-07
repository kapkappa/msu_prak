#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <cmath>
#include "defs.h"
#include "mpi.h"
#include <list>

std::list<vertex_id_t> dummy_list;
std::vector<std::list<vertex_id_t>::iterator> position_in_bucket;
std::vector<bool> vertex_was_deleted;

//std::list<vertex_id_t> bucket;
std::vector<std::list<vertex_id_t>*> buckets;

std::vector<int> send_counts;
std::vector<int> recv_counts;

typedef struct message {
    weight_t weight;
//    vertex_id_t source;
    vertex_id_t destination;
} message;

MPI_Datatype message_type;

std::vector<std::vector<message>> send_buffers;
std::vector<std::vector<message>> recv_buffers;

extern "C" void init_sssp(graph_t *G)
{
    message msg;

//    MPI_Datatype message_type;

    int nblocks = 2, blocklen[] = {1, 1};
    MPI_Aint displ[] = {0, 0};
    MPI_Datatype old_types[] = {MPI_DOUBLE, MPI_UINT32_T};
    MPI_Get_address(&msg.weight, &displ[0]);
    MPI_Get_address(&msg.destination, &displ[1]);
    displ[0] -= displ[0];
    displ[1] -= displ[0];
    MPI_Type_create_struct(nblocks, blocklen, displ, old_types, &message_type);
    MPI_Type_commit(&message_type);
}

extern "C" void finalize_sssp()
{
    MPI_Type_free(&message_type);
}


void relax(graph_t* G, weight_t* distance, vertex_id_t v, weight_t x, weight_t delta) {
#ifdef DEBUG
    std::cout << __func__ << " with params: " << v << ' ' << x << ' ' << delta << std::endl;
#endif

    if (VERTEX_OWNER(v, 0, 0) == G->rank) {
        if (x < distance[v] || distance[v] < 0) {
            size_t new_index = floor(x / delta);

            if (new_index >= buckets.size())
                buckets.resize(new_index+1, nullptr);

            if (buckets[new_index] == nullptr)
                buckets[new_index] = new std::list<vertex_id_t>;

/*
            if (distance[v] != -1 && !vertex_was_deleted[v]) {
                size_t old_index = static_cast<size_t>(distance[v] / delta);
                buckets[new_index]->splice(buckets[new_index]->end(), *buckets[old_index], position_in_bucket[v]);
            } else {
                buckets[new_index]->push_back(v);
            }

            position_in_bucket[v] = buckets[new_index]->end();
            --position_in_bucket[v];
*/

            if (distance[v] != -1) {
                size_t old_index = floor(distance[v] / delta);
                if (buckets[old_index] != nullptr)
                    std::erase(*buckets[old_index], v);
            }
            buckets[new_index]->push_back(v);

            distance[v] = x;
        }
    } else {
        if (x < distance[v] || distance[v] < 0) {
            send_buffers[VERTEX_OWNER(v, 0, 0)].push_back( {x, v} );
            send_counts[VERTEX_OWNER(v, 0, 0)]++;

//            distance[v] = x;
        }
    }
}

void synchronize(graph_t* G, weight_t* distance, double delta) {

    MPI_Alltoall(send_counts.data(), G->nproc, MPI_INT, recv_counts.data(), G->nproc, MPI_INT, MPI_COMM_WORLD);

    MPI_Request request;

    int total_cnt = 0;
    for (const auto& cnt : send_counts) total_cnt += cnt;
    int buffer_attached_size = MPI_BSEND_OVERHEAD + sizeof(message_type) * total_cnt;
    void* buffer_attached = malloc(buffer_attached_size);
    MPI_Buffer_attach(buffer_attached, buffer_attached_size);

    for (int i = 0; i < G->nproc; i++) {
        if (!send_counts[i] && i != G->rank)
            MPI_Ibsend(send_buffers[i].data(), send_counts[i], message_type, i, 0, MPI_COMM_WORLD, &request);
    }

    for (int i = 0; i < G->nproc; i++) {
        if (!recv_counts[i] && i != G->rank) {
            recv_buffers[i].resize(recv_counts[i]);
            MPI_Recv(recv_buffers[i].data(), recv_counts[i], message_type, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    for (int i = 0; i < G->nproc; i++) {
        for (const auto& msg : recv_buffers[i]) {
            relax(G, distance, msg.destination, msg.weight, delta);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Buffer_detach(&buffer_attached, &buffer_attached_size);
}

extern "C" void sssp(vertex_id_t root, graph_t *G, weight_t *distance, uint64_t *traversed_edges)
{

    double delta = (double)G->n / G->m;

    std::cout << "Start sssp with delta=" << delta << std::endl;

    uint64_t nedges = 0;

    position_in_bucket.clear();
    position_in_bucket.resize(G->local_n, dummy_list.end());

    vertex_was_deleted.clear();
    vertex_was_deleted.resize(G->local_n, false);

    send_counts.resize(G->nproc, 0);
    recv_counts.resize(G->nproc, 0);

    send_buffers.resize(G->nproc);
    recv_buffers.resize(G->nproc);

    if (VERTEX_OWNER(root, 0, 0) == G->rank) {
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
        std::cout << "light phase, current bucket: " << current_bucket << std::endl;
        while (num_buckets > 0) {

            if (current_bucket < buckets.size() && buckets[current_bucket] != nullptr) {
                std::list<vertex_id_t>& bucket = *buckets[current_bucket];

                while (!bucket.empty()) {
                    std::cout << "process bucket" << std::endl;
                    vertex_id_t u = bucket.front();
                    bucket.pop_front();

/*
                    if (!vertex_was_deleted[u]) {
                        vertex_was_deleted[u] = true;
                        deleted_vertices.push_back(u);
                    }
*/
                    deleted_vertices.insert(u);

                    //for all edges
                    weight_t dist = distance[u];
                    for (auto j = G->rowsIndices[u]; j < G->rowsIndices[u+1]; j++) {
                        if (G->weights[j] <= delta)
                            relax(G, distance, G->endV[j], dist + G->weights[j], delta);
                    }
                }
            }

//            MPI_Barrier(MPI_COMM_WORLD);
            synchronize(G, distance, delta);

            num_buckets = (current_bucket < buckets.size() && buckets[current_bucket] != nullptr && !buckets[current_bucket]->empty());
            MPI_Allreduce(&num_buckets, &num_buckets, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        }

        // heavy edges
        std::cout << "heavy phase" << std::endl;
        for (auto iter = deleted_vertices.begin(); iter != deleted_vertices.end(); ++iter) {
            vertex_id_t u = *iter;
            weight_t dist = distance[u];
            for (auto j = G->rowsIndices[u]; j < G->rowsIndices[u+1]; j++) {
                if (G->weights[j] > delta)
                    relax(G, distance, G->endV[j], dist + G->weights[j], delta);
            }
        }

        // Go to the next bucket: the current bucket must already be empty.
        current_bucket++;
    }

    // Delete all of the buckets.
    for (auto iter = buckets.begin(); iter != buckets.end(); ++iter) {
        if (*iter) {
          delete *iter;
          *iter = 0;
        }
    }
}
