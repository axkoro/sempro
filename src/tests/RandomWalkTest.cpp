#include "RandomWalkGenerator.hpp"
#include "gtest/gtest.h"
#include "StaticMinimalGraph.hpp"

TEST(RandomWalkGeneratorTest, WalksHaveCorrectLength) {
    std::vector<int> offsets = {0, 2, 4, 6, 8, 10};
    std::vector<Edge> edges = {
        {1, 1.0}, {2, 1.0}, // Node 0 -> 1,2
        {0, 1.0}, {3, 1.0}, // Node 1 -> 0,3
        {0, 1.0}, {4, 1.0}, // Node 2 -> 0,4
        {1, 1.0}, {4, 1.0}, // Node 3 -> 1,4
        {2, 1.0}, {3, 1.0}  // Node 4 -> 2,3
    };
    StaticMinimalGraph graph(offsets, edges);
    
    RandomWalkGenerator generator(graph, 5, 3); // 5 steps, 3 walks per node
    auto walks = generator.generate_walks();
    
    for (const auto& walk : walks) {
        EXPECT_EQ(walk.size(), 5); // Each walk should have 5 steps
        for (int i = 0; i < 5; ++i) {
            EXPECT_GE(walk[i], 0); // Each step should be a valid node
            EXPECT_LT(walk[i], 5);
        }
        for (int i = 0; i < 4; ++i) {
            EXPECT_NE(walk[i], walk[i + 1]); // No node should be visited twice in a row
        }
    }

    EXPECT_EQ (walks.size(), 5 * 3); // 5 nodes * 3 walks per node
}

TEST(RandomWalkGeneratorTest, TransitionsVaryBetweenRuns) {
    std::vector<int> offsets = {0, 2, 4};
    std::vector<Edge> edges = {
        {1, 1.0}, {2, 1.0},
        {0, 1.0}, {2, 1.0},
        {0, 1.0}, {1, 1.0}
    };
    StaticMinimalGraph graph(offsets, edges);
    
    RandomWalkGenerator generator1(graph, 4, 5);
    auto walks1 = generator1.generate_walks();
    
    RandomWalkGenerator generator2(graph, 4, 5);
    auto walks2 = generator2.generate_walks();
    
    EXPECT_NE(walks1, walks2); // Expect different results in different runs
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
