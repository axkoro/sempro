#include <gtest/gtest.h>

#include <unordered_set>

#include "EdgeWeightCalculator.hpp"
#include "GraphDouble.hpp"
#include "RandomWalkGenerator.hpp"
#include "SkipGram.hpp"

TEST(EdgeWeightCalculatorTest, generate_weighted_graph) {
    // Assumptions (adjust the test if any of this changes during implementation):
    // - fusion coefficient of 0.6
    // - cover depth of 2
    // - nodes themselves are not included in their covers
    // - MFS = overlap / num_features
    // - no tolerance for overlap in feature similarity calculation
    // - see Wiki for how I manually calculated the the correct weights
    double fusion_coefficient = 0.6;

    std::string edges_path = "../data/test/deepwalk/test_edges.txt";
    std::string features_path = "../data/test/deepwalk/test_features.txt";
    GraphDouble original_graph(edges_path, features_path);

    EdgeWeightCalculator ewc(original_graph, fusion_coefficient,"double");
    WeightedGraph weighted_graph = ewc.generate_weighted_graph();

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
        for (auto edge : weighted_graph.get_edges(node)) {
            int neighbour = edge.target;

            int higher = std::max(node, neighbour);
            int lower = std::min(node, neighbour);
            if (neighbour == lower) {
                count_desc_edges++;
            }

            auto it = correct_edges[higher].find(lower);
            bool edge_not_found = it == correct_edges[higher].end();
            ASSERT_FALSE(edge_not_found);

            double computed_weight = edge.weight;
            double correct_weight = it->second;
            ASSERT_NEAR(computed_weight, correct_weight, 1e-4);
        }

        ASSERT_EQ(count_desc_edges, correct_edges[node].size());
    }
}

TEST(EdgeWeightCalculatorTest, ComputeCovers) {
    // Create a simple graph for testing

    std::string edges_path = "../data/test/deepwalk/test_edges_2.txt";
    std::string features_path = "../data/test/deepwalk/test_features_2.txt";
    GraphDouble graph(edges_path, features_path);
    EdgeWeightCalculator ewc(graph, 0.5,"double");
    WeightedGraph wgraph(graph);

    // Compute covers
    std::vector<std::unordered_set<int>> covers = ewc.compute_covers(wgraph);

    // Expected covers
    std::vector<std::unordered_set<int>> expected_covers = {
        {1, 2, 3, 4},  // Cover of node 0
        {0, 2, 3, 4},  // Cover of node 1
        {0, 1, 3, 4},  // Cover of node 2
        {0, 1, 2, 4},  // Cover of node 3
        {0, 1, 2, 3}   // Cover of node 4
    };

    // Check if the computed covers match the expected covers
    ASSERT_EQ(covers.size(), expected_covers.size());
    
    ASSERT_EQ(covers, expected_covers);
    
}
TEST(EdgeWeightCalculatorTest, cover_union_intersection){

    std::string edges_path = "../data/test/deepwalk/test_edges_2.txt";
    std::string features_path = "../data/test/deepwalk/test_features_2.txt";
    GraphDouble graph(edges_path, features_path);
    EdgeWeightCalculator ewc(graph, 0.5,"double");
    WeightedGraph wgraph(graph);

    std::vector<std::unordered_set<int>> input_covers = {
        {1,2,3},
        {2,3,5},
        {1,8,5}
    };
    double mss = ewc.compute_structural_similarity(0,1,input_covers);
    //since cover union of 0 and 0 is 4 and intersection is 2 mss should be 2/4 = 2
    ASSERT_EQ(mss,0.5);
}
TEST(RandomWalkGeneratorTest, walks_have_correct_length) {
    std::vector<int> offsets = {0, 2, 4, 6, 8, 10};
    std::vector<Edge> edges = {
        {1, 1.0}, {2, 1.0},  // Node 0 -> 1,2
        {0, 1.0}, {3, 1.0},  // Node 1 -> 0,3
        {0, 1.0}, {4, 1.0},  // Node 2 -> 0,4
        {1, 1.0}, {4, 1.0},  // Node 3 -> 1,4
        {2, 1.0}, {3, 1.0}   // Node 4 -> 2,3
    };
    StaticMinimalGraph graph(offsets, edges);

    RandomWalkGenerator generator(graph, 5, 3);  // 5 steps, 3 walks per node
    auto walks = generator.generate_walks();

    for (const auto& walk : walks) {
        EXPECT_EQ(walk.size(), 5);  // Each walk should have 5 steps
        for (int i = 0; i < 5; ++i) {
            EXPECT_GE(walk[i], 0);  // Each step should be a valid node
            EXPECT_LT(walk[i], 5);
        }
        for (int i = 0; i < 3; ++i) {
            EXPECT_NE(walk[i], walk[i + 1]);  // No node should be visited twice in a row
        }
    }

    EXPECT_EQ(walks.size(), 5 * 3);  // 5 nodes * 3 walks per node
}

TEST(RandomWalkGeneratorTest, transitions_vary_between_runs) {
    std::vector<int> offsets = {0, 2, 4, 6, 8, 10};
    std::vector<Edge> edges = {
        {1, 1.0}, {2, 1.0},  // Node 0 -> 1,2
        {0, 1.0}, {3, 1.0},  // Node 1 -> 0,3
        {0, 1.0}, {4, 1.0},  // Node 2 -> 0,4
        {1, 1.0}, {4, 1.0},  // Node 3 -> 1,4
        {2, 1.0}, {3, 1.0}   // Node 4 -> 2,3
    };
    StaticMinimalGraph graph(offsets, edges);

    RandomWalkGenerator generator1(graph, 4, 5);
    auto walks1 = generator1.generate_walks();

    RandomWalkGenerator generator2(graph, 4, 5);
    auto walks2 = generator2.generate_walks();

    EXPECT_NE(walks1, walks2);  // Expect different results in different runs
}

TEST(RandomWalkGeneratorTest, same_seed_ensures_reproducibility) {
    std::vector<int> offsets = {0, 2, 4, 6};
    std::vector<Edge> edges = {{1, 0.2}, {2, 0.5}, {0, 0.5}, {2, 0.3}, {0, 0.3}, {1, 0.2}};
    StaticMinimalGraph graph(offsets, edges);

    RandomWalkGenerator generator1(graph, 4, 5, 42);
    auto walks1 = generator1.generate_walks();

    RandomWalkGenerator generator2(graph, 4, 5, 42);
    auto walks2 = generator2.generate_walks();

    EXPECT_EQ(walks1, walks2);  // Walks should be identical if the seed is the same
}

class SkipGramTest : public ::testing::Test {
   protected:
    SkipGram skipgram{10};
};

TEST_F(SkipGramTest, generate_pairs) {
    std::vector<int> walk = {1, 2, 3, 4, 5};
    int window_size = 1;
    auto pairs = skipgram.generate_pairs(walk, window_size);

    // For each center element, all elements in its window are paired (except the center node
    // itself). For window_size 1 and a walk of 5, you expect:
    // 1 (first element) + 2 + 2 + 2 + 1 (last element) = 8 pairs.
    ASSERT_EQ(pairs.size(), 8);

    EXPECT_EQ(pairs[0].in, 1);
    EXPECT_EQ(pairs[0].out, 2);

    EXPECT_EQ(pairs[1].in, 2);
    EXPECT_EQ(pairs[1].out, 1);

    EXPECT_EQ(pairs[2].in, 2);
    EXPECT_EQ(pairs[2].out, 3);

    int last = pairs.size() - 1;
    EXPECT_EQ(pairs[last].in, 5);
    EXPECT_EQ(pairs[last].out, 4);
}

// TEST(DeepWalkImputerTest, test_name) {}

int main(int, char**) {
    ::testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}