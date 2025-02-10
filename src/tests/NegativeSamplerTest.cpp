#include <gtest/gtest.h>

#include <vector>

#include "NegativeSampler.hpp"

// Test that draw_sample() always returns a valid node index.
TEST(NegativeSamplerTest, ValidSampleRange) {
    std::vector<std::vector<int>> walks = {{0, 1, 2}};
    int num_nodes = 3;
    double smoothing = 0.75;
    // Providing a fixed seed for reproducibility in tests.
    NegativeSampler nd(walks, num_nodes, smoothing, 123);

    for (int i = 0; i < 1000; ++i) {
        int sample = nd.draw_sample();
        EXPECT_GE(sample, 0);
        EXPECT_LT(sample, num_nodes);
    }
}

// Test the empirical distribution for a single walk with counts: node0:1, node1:2, node2:1.
// After smoothing with exponent 0.75, weights are approximately: 1, 1.6818, 1, so probabilities:
// node0 ≈ 1/3.6818 ≈ 0.2718, node1 ≈ 1.6818/3.6818 ≈ 0.456, node2 ≈ 0.2718.
TEST(NegativeSamplerTest, EmpiricalDistribution_SingleWalk) {
    std::vector<std::vector<int>> walks = {{0, 1, 1, 2}};
    int num_nodes = 3;
    double smoothing = 0.75;
    NegativeSampler nd(walks, num_nodes, smoothing, 456);

    const int num_samples = 100000;
    std::vector<int> counts(num_nodes, 0);
    for (int i = 0; i < num_samples; ++i) {
        int sample = nd.draw_sample();
        ++counts[sample];
    }

    double p0 = counts[0] / static_cast<double>(num_samples);
    double p1 = counts[1] / static_cast<double>(num_samples);
    double p2 = counts[2] / static_cast<double>(num_samples);

    EXPECT_NEAR(p0, 0.2718, 0.02);
    EXPECT_NEAR(p1, 0.456, 0.02);
    EXPECT_NEAR(p2, 0.2718, 0.02);
}

// Test the empirical distribution for multiple walks.
// For walks = { {0, 1}, {1, 2, 2} } the raw counts are: node0:1, node1:2, node2:2.
// After smoothing (1^0.75 = 1, 2^0.75 ≈ 1.6818):
// Total weight ≈ 1 + 1.6818 + 1.6818 = 4.3636, so probabilities:
// node0 ≈ 0.229, node1 ≈ 0.385, node2 ≈ 0.385.
TEST(NegativeSamplerTest, EmpiricalDistribution_MultipleWalks) {
    std::vector<std::vector<int>> walks = {{0, 1}, {1, 2, 2}};
    int num_nodes = 3;
    double smoothing = 0.75;
    NegativeSampler nd(walks, num_nodes, smoothing, 789);

    const int num_samples = 100000;
    std::vector<int> counts(num_nodes, 0);
    for (int i = 0; i < num_samples; ++i) {
        int sample = nd.draw_sample();
        ++counts[sample];
    }

    double p0 = counts[0] / static_cast<double>(num_samples);
    double p1 = counts[1] / static_cast<double>(num_samples);
    double p2 = counts[2] / static_cast<double>(num_samples);

    EXPECT_NEAR(p0, 0.229, 0.02);
    EXPECT_NEAR(p1, 0.385, 0.02);
    EXPECT_NEAR(p2, 0.385, 0.02);
}

// Test that a node which never appears in any walk is never sampled.
TEST(NegativeSamplerTest, NodeNeverAppearing) {
    // Only node 1 appears.
    std::vector<std::vector<int>> walks = {{1, 1, 1}};
    int num_nodes = 3;
    double smoothing = 0.75;
    NegativeSampler nd(walks, num_nodes, smoothing, 101);

    const int num_samples = 10000;
    for (int i = 0; i < num_samples; ++i) {
        int sample = nd.draw_sample();
        EXPECT_EQ(sample, 1);
    }
}

// Test that providing a fixed seed produces the same sequence of samples.
TEST(NegativeSamplerTest, FixedSeedReproducibility) {
    std::vector<std::vector<int>> walks = {{0, 1, 2, 1, 2}, {1, 2, 0, 0}};
    int num_nodes = 3;
    double smoothing = 0.75;
    int seed = 42;
    NegativeSampler nd1(walks, num_nodes, smoothing, seed);
    NegativeSampler nd2(walks, num_nodes, smoothing, seed);

    const int draws = 10;
    for (int i = 0; i < draws; ++i) {
        EXPECT_EQ(nd1.draw_sample(), nd2.draw_sample());
    }
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}