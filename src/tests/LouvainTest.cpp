#include <gtest/gtest.h>

#include "AttributedGraph.hpp"
#include "Louvain.hpp"

TEST(LouvainTest, SingleNodeGraph) {
    // Graph with 1 node (node 0) and zero edges.
    // offsets[0] = 0, offsets[1] = 0  => node 0 has no neighbors
    std::vector<int> offsets = {0, 0};
    std::vector<int> edges = {};
    Graph g(offsets, edges);

    Louvain lcd(g);
    auto communities = lcd.execute();

    ASSERT_EQ(communities.size(), 1u);
    // The single node must be in community 0 (or at least all alone).
    EXPECT_EQ(communities[0], 0);
}

TEST(LouvainTest, TwoNodeNoEdge) {
    // Graph with 2 nodes, no edges:
    // node 0 => no neighbors
    // node 1 => no neighbors
    // Hence offsets = [0,0,0], edges = {}
    std::vector<int> offsets = {0, 0, 0};
    std::vector<int> edges = {};
    Graph g(offsets, edges);

    Louvain lcd(g);
    auto communities = lcd.execute();

    ASSERT_EQ(communities.size(), 2u);
    // With no edges, typically each node remains its own community
    EXPECT_NE(communities[0], communities[1]);
}

TEST(LouvainTest, TwoNodeOneEdge) {
    // Graph with 2 nodes and 1 edge: 0 <-> 1
    // node 0 neighbors: edges[0..1) => [1]
    // node 1 neighbors: edges[1..2) => [0]
    // offsets = [0,1,2], edges = [1,0]
    std::vector<int> offsets = {0, 1, 2};
    std::vector<int> edges = {1, 0};
    Graph g(offsets, edges);

    Louvain lcd(g);
    auto communities = lcd.execute();

    ASSERT_EQ(communities.size(), 2u);
    // We expect them to be in the same community, because there's only one edge.
    EXPECT_EQ(communities[0], communities[1]);
}

TEST(LouvainTest, ThreeNodeChain) {
    // A chain of 3 nodes: 0--1--2
    // Adjacency lists:
    //   node 0 => [1]
    //   node 1 => [0,2]
    //   node 2 => [1]
    // offsets = [0,1,3,4], edges = [1,0,2,1]
    // Explanation:
    //   - node 0 adjacency is edges[0..1) => [1]
    //   - node 1 adjacency is edges[1..3) => [0,2]
    //   - node 2 adjacency is edges[3..4) => [1]
    std::vector<int> offsets = {0, 1, 3, 4};
    std::vector<int> edges = {1, 0, 2, 1};
    Graph g(offsets, edges);

    Louvain lcd(g);
    auto communities = lcd.execute();

    ASSERT_EQ(communities.size(), 3u);
    // In a small chain, the algorithm might put them all in one community,
    // or might produce a 2-1 split or even 1-1-1. We just check it runs
    // and that at least some pair is in the same community.
    bool somePairSame = ((communities[0] == communities[1]) || (communities[1] == communities[2]) ||
                         (communities[0] == communities[2]));
    EXPECT_TRUE(somePairSame);
}

TEST(LouvainTest, CompleteGraph) {
    // A complete graph with 4 nodes; each node connected to the other 3.
    // We'll use adjacency lists:
    //   node 0 => [1,2,3]
    //   node 1 => [0,2,3]
    //   node 2 => [0,1,3]
    //   node 3 => [0,1,2]
    // offsets = [0,3,6,9,12], edges = [1,2,3,0,2,3,0,1,3,0,1,2]
    std::vector<int> offsets = {0, 3, 6, 9, 12};
    std::vector<int> edges = {
        1, 2, 3,  // neighbors of node 0
        0, 2, 3,  // neighbors of node 1
        0, 1, 3,  // neighbors of node 2
        0, 1, 2   // neighbors of node 3
    };
    Graph g(offsets, edges);

    Louvain lcd(g);
    auto communities = lcd.execute();

    ASSERT_EQ(communities.size(), 4u);
    // In a complete graph of 4 nodes, the optimal partition is typically
    // a single community containing all nodes.
    int firstLabel = communities[0];
    for (int i = 1; i < 4; i++) {
        EXPECT_EQ(communities[i], firstLabel)
            << "All nodes in a complete graph should end in the same community.";
    }
}

TEST(LouvainTest, TwoComponents) {
    // 5 nodes in two components:
    //   - A triangle among {0,1,2}
    //   - A single edge connecting 3 <-> 4
    // Adjacency:
    //   node 0 => [1,2]
    //   node 1 => [0,2]
    //   node 2 => [0,1]
    //   node 3 => [4]
    //   node 4 => [3]
    //
    // offsets = [0,2,4,6,7,8], edges = [1,2,0,2,0,1,4,3]
    // Explanation:
    //   - node 0 => edges[0..2) = [1,2]
    //   - node 1 => edges[2..4) = [0,2]
    //   - node 2 => edges[4..6) = [0,1]
    //   - node 3 => edges[6..7) = [4]
    //   - node 4 => edges[7..8) = [3]
    std::vector<int> offsets = {0, 2, 4, 6, 7, 8};
    std::vector<int> edges = {1, 2, 0, 2, 0, 1, 4, 3};
    Graph g(offsets, edges);

    Louvain lcd(g);
    auto communities = lcd.execute();

    ASSERT_EQ(communities.size(), 5u);

    // We expect at least 2 distinct community labels:
    //  one for the triad {0,1,2} and another for {3,4}.
    // Check that 0,1,2 are at least in the same community:
    int comm0 = communities[0];
    EXPECT_EQ(communities[1], comm0);
    EXPECT_EQ(communities[2], comm0);

    // And that 3,4 share a different community:
    int comm3 = communities[3];
    EXPECT_NE(comm3, comm0);  // distinct from the triad
    EXPECT_EQ(communities[4], comm3);
}

// TEST(LouvainTest, LargeGraph) {
//     std::string edges_path = "../data/input/unzipped/twitch_edges.txt";
//     std::string features_path = "../data/input/unzipped/twitch_features.txt";

//     Graph g(edges_path, features_path);

//     Louvain lcd(g);
//     double pre_modularity = lcd.get_modularity();

//     EXPECT_NO_THROW(lcd.execute());
//     double post_modularity = lcd.get_modularity();

//     EXPECT_GT(post_modularity, pre_modularity);
// }

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
