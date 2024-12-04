#include <gtest/gtest.h>

#include <fstream>
#include <iostream>

#include "Graph.hpp"

class GraphTest : public testing::Test {};

TEST(GraphTest, getNumNodes) {
    std::string amazon_features = "../input/amazon/amazon_features.txt";
    EXPECT_EQ(getNumNodes(amazon_features),
              13751 + 1);  // 13751: largest node, +1: numbering begins at 0

    std::string genius_features = "../input/genius/genius_features.txt";
    EXPECT_EQ(getNumNodes(genius_features), 421960 + 1);

    std::string twitch_features = "../input/twitch/twitch_features.txt";
    EXPECT_EQ(getNumNodes(twitch_features), 9497 + 1);
}

TEST(GraphTest, getNumFeatures) {
    std::string github_features = "../input/github/github_features.txt";
    EXPECT_EQ(getNumFeatures(github_features), 128 + 1);

    std::string amazon_fraud_features = "../input/amazon_fraud/amazon_fraud_features.txt";
    EXPECT_EQ(getNumFeatures(amazon_fraud_features), 25 + 1);
}

TEST(GraphTest, readEdgesFromFile) {
    std::string edges_path = "../input/amazon/amazon_edges.txt";
    std::string features_path = "../input/amazon/amazon_features.txt";
    Graph graph(edges_path, features_path);

    // Create temporary file for output
    std::string temp_file_path = "temp_edges.txt";
    {
        std::ofstream output(temp_file_path);
        // Redirect cout to the file
        std::streambuf* old = std::cout.rdbuf(output.rdbuf());
        graph.printEdges();
        std::cout.rdbuf(old);
    }

    // Compare files line by line
    std::ifstream file1(edges_path);
    std::ifstream file2(temp_file_path);
    std::string line1, line2;
    bool files_match = true;
    int line_number = 0;

    while (std::getline(file1, line1) && std::getline(file2, line2)) {
        line_number++;
        if (line1 != line2) {
            files_match = false;
            break;
        }
    }

    // Clean up

    EXPECT_TRUE(files_match) << "Files differ at line " << line_number;

    bool more_lines = std::getline(file1, line1) || std::getline(file2, line2);
    EXPECT_FALSE(more_lines) << "Files have different lengths";

    std::remove(temp_file_path.c_str());
}

int main(int, char**) {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
