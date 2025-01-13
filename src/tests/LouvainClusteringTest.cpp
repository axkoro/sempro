#include "LouvainClustering.hpp"
#include "Graph.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>

class LouvainClusteringTest : public testing::Test {};

TEST(LouvainClusteringTest, TwoCommunities) {
    // Create a simple graph with two communities
    // Community 1: Nodes 0, 1, 2
    // Community 2: Nodes 3, 4, 5
    std::vector<int> offsets = {0, 2, 4, 6, 8, 10, 12};
    std::vector<int> edges = {
        1, 2,  // Node 0
        0, 2,  // Node 1
        0, 1,  // Node 2
        4, 5,  // Node 3
        3, 5,  // Node 4
        3, 4   // Node 5
    };

    Graph graph(offsets, edges);
    LouvainClustering louvain(graph);

    // Run the Louvain algorithm
    std::vector<int> result = louvain.execute();


    // Check if nodes are clustered correctly
    // Nodes 0, 1, 2 should belong to one community
    // Nodes 3, 4, 5 should belong to another community
    std::unordered_set<int> community1, community2;
    for (int i = 0; i < 3; ++i) {
        community1.insert(result[i]);
    }
    for (int i = 3; i < 6; ++i) {
        community2.insert(result[i]);
    }


    EXPECT_EQ(community1.size(), 1);  // All nodes in first group are in one community
    EXPECT_EQ(community2.size(), 1);  // All nodes in second group are in another community
    EXPECT_NE(*community1.begin(), *community2.begin());  // Communities are different
}

// Test Case 2: Single Community Graph
TEST(LouvainClusteringTest, SingleCommunity) {
    // Create a fully connected graph (complete graph) with 4 nodes
    // All nodes should belong to the same community
    std::vector<int> offsets = {0, 3, 6, 9, 12};
    std::vector<int> edges = {
        1, 2, 3,  // Node 0
        0, 2, 3,  // Node 1
        0, 1, 3,  // Node 2
        0, 1, 2   // Node 3
    };

    Graph graph(offsets, edges);
    LouvainClustering louvain(graph);

    // Run the Louvain algorithm
    std::vector<int> result = louvain.execute();

    // Check if all nodes are in the same community
    std::unordered_set<int> communities(result.begin(), result.end());
    EXPECT_EQ(communities.size(), 1);  // Only one community
}

// Test Case 3: Disconnected Graph
TEST(LouvainClusteringTest, DisconnectedGraph) {
    // Create a disconnected graph with 4 nodes
    // Nodes 0 and 1 are connected; Nodes 2 and 3 are connected
    std::vector<int> offsets = {0, 1, 2, 3, 4};
    std::vector<int> edges = {
        1,  // Node 0
        0,  // Node 1
        3,  // Node 2
        2   // Node 3
    };

    Graph graph(offsets, edges);
    LouvainClustering louvain(graph);

    // Run the Louvain algorithm
    std::vector<int> result = louvain.execute();

    // Check if nodes are clustered into two separate communities
    EXPECT_EQ(result[0], result[1]);  // Nodes 0 and 1 in the same community
    EXPECT_EQ(result[2], result[3]);  // Nodes 2 and 3 in the same community
    EXPECT_NE(result[0], result[2]);  // Communities are different
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
