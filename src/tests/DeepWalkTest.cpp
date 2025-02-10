#include <gtest/gtest.h>

#include <numeric>

#include "EdgeWeightCalculator.hpp"
#include "GraphDouble.hpp"
#include "RandomWalkGenerator.hpp"
#include "SkipGram.hpp"

// TEST(EdgeWeightCalculatorTest, generate_weighted_graph) {
//     // Assumptions (adjust the test if any of this changes during implementation):
//     // - fusion coefficient of 0.6
//     // - cover depth of 2
//     // - nodes themselves are not included in their covers
//     // - MFS = overlap / num_features
//     // - no tolerance for overlap in feature similarity calculation
//     // - see Wiki for how I manually calculated the the correct weights
//     double fusion_coefficient = 0.6;

//     std::string edges_path = "../data/test/deepwalk/test_edges.txt";
//     std::string features_path = "../data/test/deepwalk/test_features.txt";
//     GraphDouble original_graph(edges_path, features_path);

//     EdgeWeightCalculator ewc(original_graph, fusion_coefficient);
//     StaticMinimalGraph weighted_graph = ewc.generate_weighted_graph();

//     ASSERT_EQ(weighted_graph.get_num_nodes(), original_graph.get_num_nodes());
//     ASSERT_EQ(weighted_graph.get_num_edges(), original_graph.get_num_edges());

//     // edges are only stored once (descending: 0<->1 is stored in 1's list)
//     std::vector<std::unordered_map<int, double>> correct_edges = {
//         {},                                          // 0
//         {{0, 0.1714285714}},                         // 1
//         {{1, 0.4685714286}},                         // 2
//         {{2, 0.34}},                                 // 3
//         {{3, 0.4685714286}},                         // 4
//         {{1, 0.415}, {2, 0.415}, {4, 0.455}},        // 5
//         {{2, 0.34}, {4, 0.5885714286}, {5, 0.415}},  // 6
//         {{4, 0.2971428571}}                          // 7
//     };

//     int num_nodes = original_graph.get_num_nodes();
//     for (int node = 0; node < num_nodes; node++) {
//         int count_desc_edges = 0;
//         for (Edge computed_edge : weighted_graph.get_neighbours(node)) {
//             int neighbour = computed_edge.target;

//             int higher = std::max(node, neighbour);
//             int lower = std::min(node, neighbour);
//             if (neighbour == lower) {
//                 count_desc_edges++;
//             }

//             auto it = correct_edges[node].find(neighbour);
//             bool edge_not_found = it == correct_edges[node].end();
//             ASSERT_FALSE(edge_not_found);

//             double computed_weight = computed_edge.weight;
//             double correct_weight = it->second;
//             ASSERT_NEAR(computed_weight, correct_weight, 1e-4);
//         }

//         ASSERT_EQ(count_desc_edges, correct_edges[node].size());
//     }
// }

// TEST(RandomWalkGeneratorTest, walks_have_correct_length) {
//     std::vector<int> offsets = {0, 2, 4, 6, 8, 10};
//     std::vector<Edge> edges = {
//         {1, 1.0}, {2, 1.0},  // Node 0 -> 1,2
//         {0, 1.0}, {3, 1.0},  // Node 1 -> 0,3
//         {0, 1.0}, {4, 1.0},  // Node 2 -> 0,4
//         {1, 1.0}, {4, 1.0},  // Node 3 -> 1,4
//         {2, 1.0}, {3, 1.0}   // Node 4 -> 2,3
//     };
//     StaticMinimalGraph graph(offsets, edges);

//     RandomWalkGenerator generator(graph, 5, 3);  // 5 steps, 3 walks per node
//     auto walks = generator.generate_walks();

//     for (const auto& walk : walks) {
//         EXPECT_EQ(walk.size(), 5);  // Each walk should have 5 steps
//         for (int i = 0; i < 5; ++i) {
//             EXPECT_GE(walk[i], 0);  // Each step should be a valid node
//             EXPECT_LT(walk[i], 5);
//         }
//         for (int i = 0; i < 4; ++i) {
//             EXPECT_NE(walk[i], walk[i + 1]);  // No node should be visited twice in a row
//         }
//     }

//     EXPECT_EQ(walks.size(), 5 * 3);  // 5 nodes * 3 walks per node
// }

// TEST(RandomWalkGeneratorTest, transitions_vary_between_runs) {
//     std::vector<int> offsets = {0, 2, 4};
//     std::vector<Edge> edges = {{1, 1.0}, {2, 1.0}, {0, 1.0}, {2, 1.0}, {0, 1.0}, {1, 1.0}};
//     StaticMinimalGraph graph(offsets, edges);

//     RandomWalkGenerator generator1(graph, 4, 5);
//     auto walks1 = generator1.generate_walks();

//     RandomWalkGenerator generator2(graph, 4, 5);
//     auto walks2 = generator2.generate_walks();

//     EXPECT_NE(walks1, walks2);  // Expect different results in different runs
// }

class SkipGramTest : public ::testing::Test {
   protected:
    int num_nodes = 10;
    DeepWalkImputer::Config config;
    SkipGram* model;

    void SetUp() override {
        // Use a smaller embedding size for testing.
        config.embedding_size = 16;
        config.num_epochs = 1;
        // Default context_window is 10, so valid walks must have length ≥ 2*10+1 = 21.
        model = new SkipGram(num_nodes, config);
    }

    void TearDown() override { delete model; }
};

TEST_F(SkipGramTest, GenerateValidPairs) {
    std::vector<int> walk = {1, 2, 3, 4, 5};
    int window_size = 1;
    auto pairs = model->generate_pairs(walk, window_size);

    // For each center element, all elements in its window are paired (except the center node
    // itself). For window_size 1 and a walk of 5, you expect:
    // 1 (first element) + 2 + 2 + 2 + 1 (last element) = 8 pairs.
    ASSERT_EQ(pairs.size(), 8);

    EXPECT_EQ(pairs[0].center, 1);
    EXPECT_EQ(pairs[0].context, 2);

    EXPECT_EQ(pairs[1].center, 2);
    EXPECT_EQ(pairs[1].context, 1);

    EXPECT_EQ(pairs[2].center, 2);
    EXPECT_EQ(pairs[2].context, 3);

    int last = pairs.size() - 1;
    EXPECT_EQ(pairs[last].center, 5);
    EXPECT_EQ(pairs[last].context, 4);
}

TEST_F(SkipGramTest, ConstructorSetsEmbeddingDimensions) {
    // We assume get_embeddings returns a matrix with dimensions: num_rows == num_nodes and
    // num_cols == config.embedding_size.
    Matrix embeddings = model->get_embeddings();
    EXPECT_EQ(embeddings.num_rows(), num_nodes);
    EXPECT_EQ(embeddings.num_cols(), config.embedding_size);
}

TEST_F(SkipGramTest, TrainWithEmptyWalksDoesNotThrow) {
    std::vector<std::vector<int>> walks;
    EXPECT_THROW(model->train(walks), std::logic_error);
}

TEST_F(SkipGramTest, TrainWithValidWalksUpdatesEmbeddingsDimensions) {
    int min_length = 2 * config.context_window + 1;
    std::vector<std::vector<int>> walks;
    for (int w = 0; w < 10; ++w) {
        std::vector<int> walk;
        for (int i = 0; i < min_length; ++i) {
            // Cycle through valid node indices [0, num_nodes-1].
            walk.push_back(i % num_nodes);
        }
        walks.push_back(walk);
    }

    EXPECT_NO_THROW(model->train(walks));

    Matrix embeddings = model->get_embeddings();
    EXPECT_EQ(embeddings.num_rows(), num_nodes);
    EXPECT_EQ(embeddings.num_cols(), config.embedding_size);
}

// TEST(DeepWalkImputerTest, test_name) {}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}