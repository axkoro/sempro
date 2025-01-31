#include "DynamicMinimalGraph.hpp"

DynamicMinimalGraph::DynamicMinimalGraph(int n) : edges(n), num_edges(0) {}

DynamicMinimalGraph::DynamicMinimalGraph(const Graph& g) {
    int num_nodes = g.get_num_nodes();
    num_edges = g.get_num_edges();
    edges.resize(num_nodes);

    for (int node = 0; node < num_nodes; ++node) {
        const auto& neighbours = g.get_neighbours(node);
        for (int neighbour : neighbours) {
            if (neighbour > node) {  // to avoid double counting
                edges[node].push_back({neighbour, 1});
                edges[neighbour].push_back({node, 1});
            }
        }
    }
}

int DynamicMinimalGraph::get_num_nodes() const { return (int)edges.size(); }

int DynamicMinimalGraph::get_num_edges() const { return num_edges; }

int DynamicMinimalGraph::get_degree(int u) const {
    int deg = 0.0;
    for (auto& p : edges[u]) {
        deg += p.second;
    }
    return deg;
}

const std::vector<std::pair<int, int>>& DynamicMinimalGraph::get_neighbours(int u) const {
    return edges[u];
}

void DynamicMinimalGraph::add_edge(int u, int v, int w) {
    if (u == v) {  // self-loop
        edges[u].push_back({v, w});
        num_edges++;
    } else {
        edges[u].push_back({v, w});
        edges[v].push_back({u, w});
        num_edges++;
    }
}