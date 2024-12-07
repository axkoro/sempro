#include <gtest/gtest.h>

#include <fstream>
#include <iostream>

#include "Graph.hpp"

class GraphTest : public testing::Test {};

TEST(GraphTest, parse_node_count) {
    std::string amazon_features = "../input/amazon/amazon_features.txt";
    EXPECT_EQ(parse_node_count(amazon_features),
              13751 + 1);  // 13751: largest node, +1: numbering begins at 0

    std::string genius_features = "../input/genius/genius_features.txt";
    EXPECT_EQ(parse_node_count(genius_features), 421960 + 1);
}

TEST(GraphTest, parse_feature_count) {
    std::string github_features = "../input/github/github_features.txt";
    EXPECT_EQ(parse_feature_count(github_features), 128 + 1);

    std::string amazon_fraud_features = "../input/amazon_fraud/amazon_fraud_features.txt";
    EXPECT_EQ(parse_feature_count(amazon_fraud_features), 25 + 1);
}

TEST(GraphTest, read_edges) {
    // TODO: only check first and last lines of the files for quicker tests
    std::string edges_path = "../input/amazon/amazon_edges.txt";
    std::string features_path = "../input/amazon/amazon_features.txt";
    Graph graph(edges_path, features_path);

    // Create temporary file for output
    std::string temp_file_path = "temp_edges.txt";
    {
        std::ofstream output(temp_file_path);
        // Redirect cout to the file
        std::streambuf* old = std::cout.rdbuf(output.rdbuf());
        graph.print_edges();
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

    EXPECT_TRUE(files_match) << "Files differ at line " << line_number;

    bool more_lines = std::getline(file1, line1) || std::getline(file2, line2);
    EXPECT_FALSE(more_lines) << "Files have different lengths";

    // Clean up
    std::remove(temp_file_path.c_str());
}

TEST(GraphTest, read_features) {  // TODO: How to test this when formatting is different
    std::string edges_path = "../input/amazon/amazon_edges.txt";
    std::string features_path = "../input/amazon/amazon_features.txt";
    Graph graph(edges_path, features_path);

    std::string temp_file_path = "test_output_features.txt";
    {
        std::ofstream output(temp_file_path);
        // Redirect cout to the file
        std::streambuf* old = std::cout.rdbuf(output.rdbuf());
        graph.print_features();
        std::cout.rdbuf(old);
    }
}

int main(int, char**) {
    ::testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}
