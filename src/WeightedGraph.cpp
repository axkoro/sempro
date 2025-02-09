#include "WeightedGraph.hpp"

#include <queue>
#include <unordered_set>

// --- WeightedEdgeIterator Implementation ---

WeightedEdgeIterator::WeightedEdgeIterator(WeightedGraph* wg, size_t idx)
    : weighted_graph(wg), index(idx), current_node(0) {
    // Advance current_node until we find the node corresponding to index.
    while (current_node + 1 < weighted_graph->graph.offsets.size() &&
           index >= weighted_graph->graph.offsets[current_node + 1]) {
        ++current_node;
    }
}

WeightedEdgeIterator& WeightedEdgeIterator::operator++() {
    ++index;
    if (current_node + 1 < weighted_graph->graph.offsets.size() &&
        index >= weighted_graph->graph.offsets[current_node + 1]) {
        ++current_node;
    }
    return *this;
}

bool WeightedEdgeIterator::operator!=(const WeightedEdgeIterator& other) const {
    return index != other.index;
}

double& WeightedEdgeIterator::operator*() { return weighted_graph->edge_weights[index]; }

std::pair<int, int> WeightedEdgeIterator::get_edge() const {
    return {static_cast<int>(current_node), weighted_graph->graph.edges[index]};
}

// --- WeightedGraph Implementation ---

WeightedGraph::WeightedGraph(Graph& graph) : graph(graph), edge_weights(graph.edges.size(), 1.0) {}

WeightedEdgeIterator WeightedGraph::begin() { return WeightedEdgeIterator(this, 0); }

WeightedEdgeIterator WeightedGraph::end() {
    return WeightedEdgeIterator(this, edge_weights.size());
}

std::vector<WeightedGraph::Edge> WeightedGraph::get_edges(int node) const {
    if (!(is_valid_node(node))) throw std::logic_error("Node does not exist");

    std::vector<Edge> neighbours(graph.offsets[node + 1] - graph.offsets[node]);
    size_t num_neighbours = neighbours.size();
    size_t offset = graph.offsets[node];
    for (size_t i = 0; i < num_neighbours; i++) {
        neighbours[i].target = graph.edges[offset + i];
        neighbours[i].weight = edge_weights[offset + i];
    }

    return neighbours;
}

std::vector<WeightedGraph::Edge> WeightedGraph::get_edges(int node, int depth) const {
    if (!is_valid_node(node)) throw std::logic_error("Node does not exist");

    std::unordered_set<int> visited;
    std::queue<int> frontier;
    frontier.push(node);
    visited.insert(node);

    std::vector<Edge> result;

    for (int d = 0; d < depth; ++d) {
        int frontierSize = frontier.size();
        for (int i = 0; i < frontierSize; ++i) {
            int curr = frontier.front();
            frontier.pop();

            std::vector<Edge> currEdges = get_edges(curr);
            for (const auto& edge : currEdges) {
                if (!visited.count(edge.target)) {
                    visited.insert(edge.target);
                    frontier.push(edge.target);
                }
                result.push_back(edge);
            }
        }
    }

    return result;
}

// --- Forwarded Graph Methods ---

int WeightedGraph::get_num_nodes() const { return graph.get_num_nodes(); }

int WeightedGraph::get_num_features() const { return graph.get_num_features(); }

int WeightedGraph::get_num_edges() const { return graph.get_num_edges(); }

bool WeightedGraph::get_bool_feature(int node, int feature) const {
    return graph.get_bool_feature(node, feature);
}

double WeightedGraph::get_double_feature(int node, int feature) const {
    return graph.get_double_feature(node, feature);
}

int WeightedGraph::get_int_feature(int node, int feature) const {
    return graph.get_int_feature(node, feature);
}

int WeightedGraph::get_label(int node) const { return graph.get_label(node); }

std::vector<int> WeightedGraph::get_missing_features(int node) const {
    return graph.get_missing_features(node);
}

int WeightedGraph::get_degree(int node) const { return graph.get_degree(node); }

void WeightedGraph::set_bool_feature(int node, int feature, bool value) {
    graph.set_bool_feature(node, feature, value);
}

void WeightedGraph::set_double_feature(int node, int feature, double value) {
    graph.set_double_feature(node, feature, value);
}

void WeightedGraph::set_int_feature(int node, int feature, int value) {
    graph.set_int_feature(node, feature, value);
}

void WeightedGraph::set_missing(int node, int feature, bool value) {
    graph.set_missing(node, feature, value);
}

bool WeightedGraph::has_edge(int source, int target) const {
    return graph.has_edge(source, target);
}

bool WeightedGraph::is_missing(int node, int feature) const {
    return graph.is_missing(node, feature);
}

bool WeightedGraph::is_valid_node(int node) const { return graph.is_valid_node(node); }
