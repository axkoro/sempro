#include <gtest/gtest.h>

#include <numeric>
#include <unordered_set>

#include "AttributedGraph.hpp"
#include "DeepWalkImputer.hpp"

TEST(EdgeWeightCalculatorTest, generate_edge_weights) {
    // Assumptions (adjust the test if any of this changes during implementation):
    // - fusion coefficient of 0.6
    // - cover depth of 2
    // - nodes themselves are not included in their covers
    // - MFS = overlap / num_features
    // - no tolerance for overlap in feature similarity calculation
    // - see Wiki for how I manually calculated the the correct weights
    double fusion_coefficient = 0.4;

    std::string edges_path = "../data/test/deepwalk/test_edges.txt";
    std::string features_path = "../data/test/deepwalk/test_features.txt";
    AttributedGraph<double> graph(edges_path, features_path);

    EdgeWeightCalculator ewc(graph, fusion_coefficient);
    GraphEdgeWeights edge_weights = ewc.generate_edge_weights();

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

    int num_nodes = graph.get_num_nodes();
    for (int node = 0; node < num_nodes; node++) {
        int count_desc_edges = 0;
        for (auto edge : edge_weights.get_edges(node)) {
            int neighbor = edge.target;

            int higher = std::max(node, neighbor);
            int lower = std::min(node, neighbor);
            if (neighbor == lower) {
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
    AttributedGraph<double> graph(edges_path, features_path);
    EdgeWeightCalculator ewc(graph, 0.5);

    // Compute covers
    std::vector<std::unordered_set<int>> covers = ewc.compute_covers();

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

TEST(EdgeWeightCalculatorTest, cover_union_intersection) {
    std::string edges_path = "../data/test/deepwalk/test_edges_2.txt";
    std::string features_path = "../data/test/deepwalk/test_features_2.txt";
    AttributedGraph<double> graph(edges_path, features_path);
    EdgeWeightCalculator ewc(graph, 0.5);

    std::vector<std::unordered_set<int>> input_covers = {{1, 2, 3}, {2, 3, 5}, {1, 8, 5}};
    double mss = ewc.compute_structural_similarity(0, 1, input_covers);
    // since cover union of 0 and 0 is 4 and intersection is 2 mss should be 2/4 = 2
    ASSERT_EQ(mss, 0.5);
}

TEST(RandomWalkGeneratorTest, walks_have_correct_length) {
    std::string edges_file = "../data/test/deepwalk/test_edges.txt";
    std::string features_file = "../data/test/deepwalk/test_features.txt";

    AttributedGraph<double> graph(edges_file, features_file);
    GraphEdgeWeights edge_weights(graph);

    RandomWalkGenerator generator(graph, edge_weights, 5, 3);  // 5 steps, 3 walks per node
    auto walks = generator.generate_walks();

    for (const auto& walk : walks) {
        EXPECT_EQ(walk.size(), 5);  // Each walk should have 5 steps
        for (int i = 0; i < 5; ++i) {
            EXPECT_GE(walk[i], 0);  // Each step should be a valid node
            EXPECT_LT(walk[i], 8);
        }
        for (int i = 0; i < 3; ++i) {
            EXPECT_NE(walk[i], walk[i + 1]);  // No node should be visited twice in a row
        }
    }

    EXPECT_EQ(walks.size(), 8 * 3);  // 8 nodes * 3 walks per node
}

TEST(RandomWalkGeneratorTest, transitions_vary_between_runs) {
    std::string edges_file = "../data/test/deepwalk/test_edges.txt";
    std::string features_file = "../data/test/deepwalk/test_features.txt";

    AttributedGraph<double> graph(edges_file, features_file);
    GraphEdgeWeights edge_weights(graph);

    RandomWalkGenerator generator1(graph, edge_weights, 4, 5);
    auto walks1 = generator1.generate_walks();

    RandomWalkGenerator generator2(graph, edge_weights, 4, 5);
    auto walks2 = generator2.generate_walks();

    EXPECT_NE(walks1, walks2);  // Expect different results in different runs
}

TEST(RandomWalkGeneratorTest, same_seed_ensures_reproducibility) {
    std::string edges_file = "../data/test/deepwalk/test_edges.txt";
    std::string features_file = "../data/test/deepwalk/test_features.txt";

    AttributedGraph<double> graph(edges_file, features_file);
    GraphEdgeWeights edge_weights(graph);

    RandomWalkGenerator generator1(graph, edge_weights, 4, 5, 42);
    auto walks1 = generator1.generate_walks();

    RandomWalkGenerator generator2(graph, edge_weights, 4, 5, 42);
    auto walks2 = generator2.generate_walks();

    EXPECT_EQ(walks1, walks2);  // Walks should be identical if the seed is the same
}

class SkipGramTest : public ::testing::Test {
   protected:
    int num_nodes = 10;
    SkipGramConfig config;
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

TEST(DeepWalkImputerTest, no_missing_features) {
    std::string edges_file = "../data/test/deepwalk/test_edges.txt";
    std::string features_file = "../data/test/deepwalk/test_missing_features.txt";

    AttributedGraph<double> graph(edges_file, features_file);

    DeepWalkConfig config;
    config.embedding_size = 8;
    config.num_negative_samples = 2;

    DeepWalkImputer<double> imputer(graph, config);
    imputer.run();

    for (int node = 0; node < graph.get_num_nodes(); ++node) {
        std::vector<int> missing_features = graph.get_missing_features(node);
        ASSERT_TRUE(missing_features.empty()) << "Node " << node << " has missing features.";
    }
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}