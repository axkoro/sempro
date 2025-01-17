#include <vector>

#include "Graph.hpp"

class MinimalGraph {
   private:
    std::vector<std::vector<std::pair<int, double>>> edges;
    int num_edges;

   public:
    explicit MinimalGraph(int n) : edges(n), num_edges(0) {}

    explicit MinimalGraph(const Graph& g) {
        int num_nodes = g.get_num_nodes();
        num_edges = g.get_num_edges();
        edges.resize(num_nodes);

        for (int node = 0; node < num_nodes; ++node) {
            const auto& neighbours = g.get_neighbours(node);
            for (int neighbour : neighbours) {
                if (neighbour > node) {  // to avoid double counting
                    edges[node].push_back({neighbour, 1.0});
                    edges[neighbour].push_back({node, 1.0});
                }
            }
        }
    }

    int get_num_nodes() const { return (int)edges.size(); }

    int get_num_edges() const { return num_edges; }

    // Return adjacency list for node "u"
    // Each entry is (neighbor, weight)
    const std::vector<std::pair<int, double>>& get_neighbours(int u) const { return edges[u]; }

    // Return sum of weights of edges from node "u"  (the "weighted degree")
    double get_degree(int u) const {
        double deg = 0.0;
        for (auto& p : edges[u]) {
            deg += p.second;
        }
        return deg;
    }

    // Add an edge with a given weight in an undirected manner
    void add_edge(int u, int v, double w) {
        if (u == v) {  // self-loop
            edges[u].push_back({v, w});
            num_edges++;
        } else {
            edges[u].push_back({v, w});
            edges[v].push_back({u, w});
            num_edges++;
        }
    }
};