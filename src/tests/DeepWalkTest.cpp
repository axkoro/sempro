#include <gtest/gtest.h>

#include "EdgeWeightCalculator.hpp"
#include "GraphDouble.hpp"

// Assumptions (adjust the test if any of this changes during implementation):
// - fusion coefficient of 0.6
// - cover depth of 2
// - nodes themselves are not included in their covers
// - MFS = overlap / num_features
// - no tolerance for overlap in feature similarity calculation
// - see Wiki for how I manually calculated the the correct weights
TEST(EdgeWeightCalculatorTest, generate_weighted_graph) {
    double fusion_coefficient = 0.6;

    std::string edges_path = "../data/test/deepwalk/test_edges.txt";
    std::string features_path = "../data/test/deepwalk/test_features.txt";
    GraphDouble original_graph(edges_path, features_path);

    EdgeWeightCalculator ewc(original_graph, fusion_coefficient);
    StaticMinimalGraph weighted_graph = ewc.generate_weighted_graph();

    ASSERT_EQ(weighted_graph.get_num_nodes(), original_graph.get_num_nodes());
    ASSERT_EQ(weighted_graph.get_num_edges(), original_graph.get_num_edges());

    // edges are only stored once (descending: 0<->1 is stored in 1's list)
    std::vector<std::unordered_map<int, double>> correct_edges = {
        {},                                          // 0
        {{0, 0.1714285714}},                         // 1
        {{1, 0.4685714286}},                         // 2
        {{2, 0.34}},                                 // 3
        {{3, 0.4685714286}},                         // 4
        {{1, 0.415}, {2, 0.415}, {4, 0.455}},        // 5
        {{2, 0.34}, {4, 0.5885714286}, {5, 0.415}},  // 6
        {{4, 0.2971428571}}                          // 7
    };

    int num_nodes = original_graph.get_num_nodes();
    for (int node = 0; node < num_nodes; node++) {
        int count_desc_edges = 0;
        for (Edge computed_edge : weighted_graph.get_neighbours(node)) {
            int neighbour = computed_edge.target;

            int higher = std::max(node, neighbour);
            int lower = std::min(node, neighbour);
            if (neighbour == lower) {
                count_desc_edges++;
            }

            auto it = correct_edges[node].find(neighbour);
            bool edge_not_found = it == correct_edges[node].end();
            ASSERT_FALSE(edge_not_found);

            double computed_weight = computed_edge.weight;
            double correct_weight = it->second;
            ASSERT_NEAR(computed_weight, correct_weight, 1e-4);
        }

        ASSERT_EQ(count_desc_edges, correct_edges[node].size());
    }
}

TEST(RandomWalkGeneratorTest, test_name) {}

TEST(SkipGramTest, test_name) {}

TEST(DeepWalkImputerTest, test_name) {}