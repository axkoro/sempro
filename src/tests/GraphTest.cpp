#include <gtest/gtest.h>

#include <fstream>
#include <iostream>

#include "Graph.hpp"

class GraphTest : public testing::Test {};

TEST(GraphTest, getNumNodes) {
    std::string amazon_edges = "../input/amazon/amazon_edges.txt";
    EXPECT_EQ(getNumNodes(amazon_edges), 13751 + 1);

    std::string genius_edges = "../input/genius/genius_edges.txt";
    EXPECT_EQ(getNumNodes(genius_edges), 421960 + 1);

    std::string twitch_edges = "../input/twitch/twitch_edges.txt";
    EXPECT_EQ(getNumNodes(twitch_edges), 9497 + 1);
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
