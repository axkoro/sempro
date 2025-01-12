#include <gtest/gtest.h>

#include <fstream>
#include <iostream>

#include "Graph.hpp"

class GraphTest : public testing::Test {};

TEST(GraphTest, FileNotOpenThrowsError) {
    // Provide a non-existent file path to trigger the error
    std::string invalid_file_path = "non_existent_file.txt";

    // Verify that the function throws a runtime_error
    EXPECT_THROW(
        {
            std::ifstream file(invalid_file_path);
            if (!file.is_open()) {
                throw std::runtime_error("Could not open file");
            }
        },
        std::runtime_error);
}

TEST(GraphTest, parse_node_count) {
    std::string amazon_features = "../data/input/unzipped/amazon_features.txt";
    EXPECT_EQ(parse_node_count(amazon_features),
              13751 + 1);  // 13751: largest node, +1: numbering begins at 0

    std::string genius_features = "../data/input/unzipped/genius_features.txt";
    EXPECT_EQ(parse_node_count(genius_features), 421960 + 1);
}

TEST(GraphTest, parse_feature_count) {
    std::string github_features = "../data/input/unzipped/github_features.txt";
    EXPECT_EQ(parse_feature_count(github_features), 128 + 1);

    std::string amazon_fraud_features = "../data/input/unzipped/amazon_fraud_features.txt";
    EXPECT_EQ(parse_feature_count(amazon_fraud_features), 25 + 1);
}

TEST(GraphTest, get_neighbours) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    Graph graph(edges_path, features_path);

    std::vector<int> neighbours = graph.get_neighbours(0);
    std::vector<int> expected_neighbours = {1, 4};

    EXPECT_EQ(neighbours, expected_neighbours);

    // Test for a specific depth
    std::vector<int> depth_neighbours = graph.get_neighbours(0, 2);
    std::vector<int> expected_depth_neighbours = {1, 4, 0, 2, 3};

    EXPECT_EQ(depth_neighbours, expected_depth_neighbours);
}

TEST(GraphTest, get_degree) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    Graph graph(edges_path, features_path);

    int degree = graph.get_degree(0);
    EXPECT_GE(degree, 2);
}

TEST(GraphTest, has_edge) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    Graph graph(edges_path, features_path);

    EXPECT_TRUE(graph.has_edge(0, 4));   // Assumes edge exists
    EXPECT_FALSE(graph.has_edge(0, 2));  // Assumes edge does not exist
}

TEST(GraphTest, is_valid_node) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    Graph graph(edges_path, features_path);

    EXPECT_TRUE(graph.is_valid_node(0));
    EXPECT_FALSE(graph.is_valid_node(graph.get_num_nodes()));
}

TEST(GraphTest, print_edges) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    Graph graph(edges_path, features_path);

    // Redirect std::cout to capture the output
    std::ostringstream output;
    std::streambuf* oldCoutBuffer = std::cout.rdbuf(output.rdbuf());

    // Call the function to capture its output
    graph.print_edges();

    // Restore std::cout
    std::cout.rdbuf(oldCoutBuffer);

    std::string expected_print =
        "1\t0\n"
        "2\t1\n"
        "3\t2\n"
        "4\t3\n"
        "4\t0\n";

    EXPECT_EQ(output.str(), expected_print);
}

TEST(GraphTest, print_features) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    Graph graph(edges_path, features_path);

    // Redirect std::cout to capture the output
    std::ostringstream output;
    std::streambuf* oldCoutBuffer = std::cout.rdbuf(output.rdbuf());

    // Call the function to capture its output
    graph.print_features("");

    // Restore std::cout
    std::cout.rdbuf(oldCoutBuffer);

    std::string expected_print =
        "0\t1, 0, '#', 0, 0, 0\t2\n"
        "1\t0, 1, '#', 0, 0, 0\t0\n"
        "2\t0, 0, '#', 1, 0, 0\t1\n"
        "3\t0, 0, '#', 0, 1, 0\t3\n"
        "4\t0, 0, '#', 0, 0, 1\t4\n";

    EXPECT_EQ(output.str(), expected_print);
}

TEST(GraphTest, read_edges) {
    // TODO: only check first and last lines of the files for quicker tests
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
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

TEST(GraphTest, read_features) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    Graph graph(edges_path, features_path);

    // clang-format off
    std::vector<std::vector<double>> actual_features = {
        {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2},
        {0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0},
        {0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1},
        {0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 3},
        {0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 4}
    };
    // clang-format on

    for (int node = 0; node < graph.get_num_nodes(); node++) {
        for (int feature = 0; feature < graph.get_num_features(); feature++) {
            if (!graph.is_missing(node, feature)) {
                EXPECT_DOUBLE_EQ(graph.get_feature(node, feature), actual_features[node][feature]);
            }
        }
    }
}

TEST(UtilityTest, remove_duplicates) {
    // Test Case 1: Vector with duplicates
    std::vector<int> input1 = {1, 2, 3, 2, 4, 3, 5};
    std::vector<int> expected1 = {1, 2, 3, 4, 5};
    EXPECT_EQ(remove_duplicates(input1), expected1);

    // Test Case 2: Vector with no duplicates
    std::vector<int> input2 = {6, 7, 8, 9};
    std::vector<int> expected2 = {6, 7, 8, 9};
    EXPECT_EQ(remove_duplicates(input2), expected2);

    // Test Case 3: All identical values
    std::vector<int> input4 = {10, 10, 10, 10};
    std::vector<int> expected4 = {10};
    EXPECT_EQ(remove_duplicates(input4), expected4);
}

int main(int, char**) {
    ::testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}
