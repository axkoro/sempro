#include "DynamicGraph.hpp"

DynamicGraph::DynamicGraph(int n) : edges(n), num_edges(0) {}

DynamicGraph::DynamicGraph(const Graph& g) {
    int num_nodes = g.get_num_nodes();
    num_edges = g.get_num_edges();
    edges.resize(num_nodes);

    for (int node = 0; node < num_nodes; ++node) {
        auto neighbors = g.get_neighbors(node);
        for (int neighbor : neighbors) {
            if (neighbor > node) {  // to avoid double counting
                edges[node].emplace_back(neighbor, 1);
                edges[neighbor].emplace_back(node, 1);
            }
        }
    }
}

int DynamicGraph::get_num_nodes() const { return edges.size(); }

int DynamicGraph::get_num_edges() const { return num_edges; }

int DynamicGraph::get_degree(int u) const {
    int deg = 0.0;
    for (const auto& edge : edges[u]) {
        deg += edge.second;
    }
    return deg;
}

const std::vector<std::pair<int, int>>& DynamicGraph::get_neighbors(int u) const {
    return edges[u];
}

void DynamicGraph::add_edge(int u, int v, int w) {
    if (u == v) {  // self-loop
        edges[u].emplace_back(v, w);
        num_edges++;
    } else {
        edges[u].emplace_back(v, w);
        edges[v].emplace_back(u, w);
        num_edges++;
    }
}