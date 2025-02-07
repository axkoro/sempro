#include "StaticMinimalGraph.hpp"
#include <stdexcept>

int StaticMinimalGraph::get_num_nodes() const {
    return offsets.size() - 1;
}

int StaticMinimalGraph::get_num_edges() const {
    return edges.size() / 2;
}

int StaticMinimalGraph::get_degree(int u) const {
    if (u < 0 || u >= get_num_nodes()) {
        throw std::logic_error("Node does not exist");
    }
    return offsets[u + 1] - offsets[u];
}

const std::vector<Edge>& StaticMinimalGraph::get_neighbours(int u) const {
    if (u < 0 || u >= get_num_nodes()) {
        throw std::logic_error("Node does not exist");
    }
    int start = offsets[u];
    int end = offsets[u + 1];
    return {edges.begin() + start, edges.begin() + end};
}
