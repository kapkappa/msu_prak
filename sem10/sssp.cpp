#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <cmath>
#include "defs.h"

using namespace std;

extern "C" void init_sssp(graph_t *G)
{
}

extern "C" void finalize_sssp()
{
}

vector<pair<vertex_id_t, weight_t>> findRequests(vector<vertex_id_t> V, graph_t* G, weight_t * dist, bool is_light, double delta, uint64_t*nedges) {
    cout << __func__ << endl;
    vector<pair<vertex_id_t, weight_t>> result;
    for (const vertex_id_t& v : V) {
        for (auto j = G->rowsIndices[v]; j < G->rowsIndices[v+1]; j++) {
            *nedges++;
            vertex_id_t dest = G->endV[j];
            if (is_light && G->weights[j] < delta || !is_light && G->weights[j] >= delta) {
                result.emplace_back(make_pair(dest, dist[v] + G->weights[j]));
            }
        }
    }
    return result;
}

vector<pair<vertex_id_t, weight_t>> findRequests(set<vertex_id_t> V, graph_t* G, weight_t * dist, bool is_light, double delta, uint64_t*nedges) {
    cout << __func__ << endl;
    vector<pair<vertex_id_t, weight_t>> result;
    for (const vertex_id_t& v : V) {
        for (auto j = G->rowsIndices[v]; j < G->rowsIndices[v+1]; j++) {
            *nedges++;
            vertex_id_t dest = G->endV[j];
            if (is_light && G->weights[j] < delta || !is_light && G->weights[j] >= delta) {
                result.emplace_back(make_pair(dest, dist[v] + G->weights[j]));
            }
        }
    }
    return result;
}

void relax(vertex_id_t v, weight_t x, weight_t* dist, vector<vector<vertex_id_t>>& B, double delta) {
    cout << __func__ << endl;
    if (x < dist[v] || dist[v] == -1) {
        std::erase(B[floor(dist[v] / delta)], v);
        B[floor(x / delta)].push_back(v);
        dist[v] = x;
    }
}

void relaxRequests(const vector<pair<vertex_id_t, weight_t>>& requests, weight_t* dist, vector<vector<vertex_id_t>>& B, double delta) {
    cout << __func__ << endl;
    for (const auto& req : requests) {
        relax(req.first, req.second, dist, B, delta);
    }
}

extern "C" void sssp(vertex_id_t root, graph_t *G, weight_t *dist, uint64_t *traversed_edges)
{
    cout << "Start" << endl;
    dist[root] = 0;
    vector<vector<vertex_id_t>> B;
    double delta = 1 / G->avg_vertex_degree;
    uint64_t nedges = 0;
    B.emplace_back(vector(1, root));
    int sum = 1;

    while (sum != 0) {

        int i = B.size() - 1;
        for (int _i = 0; _i < B.size(); _i++)
            if (!B[_i].empty()) {i = _i; break;}

        cout << "Light phase" << endl;

        set<vertex_id_t> R;

        while (!B[i].empty()) {
            auto Req = findRequests(B[i], G, dist, true, delta, &nedges);
            for (const auto& v : B[i])
                R.insert(v);
            B[i].clear();
            relaxRequests(Req, dist, B, delta);
        }

        cout << "Heavy phase" << endl;

        auto Req = findRequests(R, G, dist, false, delta, &nedges);
        relaxRequests(Req, dist, B, delta);

        sum = 0;
        for (const auto& b : B)
            if (!b.empty()) sum = 1;
    }

    *traversed_edges = nedges;
    cout << "End" << endl;
}

