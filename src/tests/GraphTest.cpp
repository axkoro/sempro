#include <gtest/gtest.h>

#include <iostream>

#include "Graph.hpp"

class GraphTest : public testing::Test {};

TEST(GraphTest, getNumNodes) {
    std::string amazon_edges = "../input/amazon/amazon_edges.txt";
    EXPECT_EQ(getNumNodes(amazon_edges), 13751);

    std::string genius_edges = "../input/genius/genius_edges.txt";
    EXPECT_EQ(getNumNodes(genius_edges), 421960);

    std::string twitch_edges = "../input/twitch/twitch_edges.txt";
    EXPECT_EQ(getNumNodes(twitch_edges), 9497);
}

// TEST(GraphTest, readEdgesFromFile) {
//     std::string edges_path = "input/amazon/amazon_edges.txt";
//     std::string features_path = "input/amazon/amazon_features.txt";
//     Graph graph(edges_path, features_path);
// }

int main(int, char**) {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
