#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "AttributedGraph.hpp"
#include "GraphEdgeWeights.hpp"

class GraphTest : public testing::Test {};

TEST(GraphTest, file_not_open_throws_error) {
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

TEST(GraphTest, invalid_node_index_throws) {
    AttributedGraph<double> graph("../data/test/graph/edges_example.txt",
                                  "../data/test/graph/features_example.txt");

    // Node index out of range
    EXPECT_THROW(graph.get_feature(graph.get_num_nodes(), 0), GraphException);
}

TEST(GraphTest, invalid_feature_index_throws) {
    AttributedGraph<double> graph("../data/test/graph/edges_example.txt",
                                  "../data/test/graph/features_example.txt");

    // Feature index out of range
    EXPECT_THROW(graph.get_feature(0, graph.get_num_features()), GraphException);
}

TEST(GraphTest, parse_node_count_from_feature_file) {
    std::string edges_file = "../data/test/graph/edges_example.txt";
    std::string features_file = "../data/test/graph/features_example.txt";
    AttributedGraph<int> graph(edges_file, features_file);
    EXPECT_EQ(graph.get_num_edges(),
              4 + 1);  // 4 = largest node index, +1 because numbering begins at 0
}

TEST(GraphTest, parse_feature_count) {
    std::string edges_file = "../data/test/graph/edges_example.txt";
    std::string features_file = "../data/test/graph/features_example.txt";
    AttributedGraph<int> graph(edges_file, features_file);
    EXPECT_EQ(graph.get_num_features(), 6);
}

// FIXME: Currently checks reading of both edges and features (should only load edges)
TEST(GraphTest, read_edges) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    AttributedGraph<double> graph(edges_path, features_path);

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

TEST(GraphTest, print_edges) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    AttributedGraph<double> graph(edges_path, features_path);

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

TEST(GraphTest, read_bool_features) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    AttributedGraph<bool> graph(edges_path, features_path);

    // clang-format off
    std::vector<std::vector<bool>> actual_features = {
        {true, false, false, false, false, false},
        {false, true, false, false, false, false},
        {false, false, false, true, false, false},
        {false, false, false, false, true, false},
        {false, false, false, false, false, true}
    };

    std::vector<std::vector<bool>> actual_missing = {
        {false, false, true, false, false, false},
        {false, false, true, false, false, false},
        {false, false, true, false, false, false},
        {false, false, true, false, false, false},
        {false, false, true, false, false, false}
    };
    // clang-format on

    std::vector<int> actual_labels = {2, 0, 1, 3, 4};

    for (int node = 0; node < graph.get_num_nodes(); node++) {
        for (int feature = 0; feature < graph.get_num_features(); feature++) {
            // check if the correct features are loaded as missing
            EXPECT_EQ(graph.is_missing(node, feature), actual_missing[node][feature]);

            // check features
            if (!graph.is_missing(node, feature)) {
                EXPECT_EQ(graph.get_feature(node, feature), actual_features[node][feature]);
            }
        }
    }

    // check labels
    for (int node = 0; node < graph.get_num_nodes(); node++) {
        EXPECT_EQ(graph.get_label(node), actual_labels[node]);
    }
}

TEST(GraphTest, read_int_features) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    AttributedGraph<int> graph(edges_path, features_path);

    // clang-format off
    std::vector<std::vector<int>> actual_features = {
        {1, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 1}
    };

    std::vector<std::vector<bool>> actual_missing = {
        {false, false, true, false, false, false},
        {false, false, true, false, false, false},
        {false, false, true, false, false, false},
        {false, false, true, false, false, false},
        {false, false, true, false, false, false}
    };
    // clang-format on

    std::vector<int> actual_labels = {2, 0, 1, 3, 4};

    for (int node = 0; node < graph.get_num_nodes(); node++) {
        for (int feature = 0; feature < graph.get_num_features(); feature++) {
            // check if the correct features are loaded as missing
            EXPECT_EQ(graph.is_missing(node, feature), actual_missing[node][feature]);

            // check features
            if (!graph.is_missing(node, feature)) {
                EXPECT_EQ(graph.get_feature(node, feature), actual_features[node][feature]);
            }
        }
    }

    // check labels
    for (int node = 0; node < graph.get_num_nodes(); node++) {
        EXPECT_EQ(graph.get_label(node), actual_labels[node]);
    }
}

TEST(GraphTest, read_double_features) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    AttributedGraph<double> graph(edges_path, features_path);

    // clang-format off
    std::vector<std::vector<double>> actual_features = {
        {1.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, 1.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 1.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 1.0}
    };

    std::vector<std::vector<bool>> actual_missing = {
        {false, false, true, false, false, false},
        {false, false, true, false, false, false},
        {false, false, true, false, false, false},
        {false, false, true, false, false, false},
        {false, false, true, false, false, false}
    };
    // clang-format on

    std::vector<int> actual_labels = {2, 0, 1, 3, 4};

    for (int node = 0; node < graph.get_num_nodes(); node++) {
        for (int feature = 0; feature < graph.get_num_features(); feature++) {
            // check if the correct features are loaded as missing
            EXPECT_EQ(graph.is_missing(node, feature), actual_missing[node][feature]);

            // check features
            if (!graph.is_missing(node, feature)) {
                EXPECT_DOUBLE_EQ(graph.get_feature(node, feature), actual_features[node][feature]);
            }
        }
    }

    // check labels
    for (int node = 0; node < graph.get_num_nodes(); node++) {
        EXPECT_EQ(graph.get_label(node), actual_labels[node]);
    }
}

TEST(GraphTest, print_bool_features) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    AttributedGraph<bool> graph(edges_path, features_path);

    // Redirect std::cout to capture the output
    std::ostringstream output;
    std::streambuf* oldCoutBuffer = std::cout.rdbuf(output.rdbuf());

    // Call the function to capture its output
    graph.print_features();

    // Restore std::cout
    std::cout.rdbuf(oldCoutBuffer);

    std::string expected_print =
        "0\t1, 0, #, 0, 0, 0\t2\n"
        "1\t0, 1, #, 0, 0, 0\t0\n"
        "2\t0, 0, #, 1, 0, 0\t1\n"
        "3\t0, 0, #, 0, 1, 0\t3\n"
        "4\t0, 0, #, 0, 0, 1\t4\n";

    EXPECT_EQ(output.str(), expected_print);
}

TEST(GraphTest, print_int_features) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    AttributedGraph<int> graph(edges_path, features_path);

    // Redirect std::cout to capture the output
    std::ostringstream output;
    std::streambuf* oldCoutBuffer = std::cout.rdbuf(output.rdbuf());

    // Call the function to capture its output
    graph.print_features();

    // Restore std::cout
    std::cout.rdbuf(oldCoutBuffer);

    std::string expected_print =
        "0\t1, 0, #, 0, 0, 0\t2\n"
        "1\t0, 1, #, 0, 0, 0\t0\n"
        "2\t0, 0, #, 1, 0, 0\t1\n"
        "3\t0, 0, #, 0, 1, 0\t3\n"
        "4\t0, 0, #, 0, 0, 1\t4\n";

    EXPECT_EQ(output.str(), expected_print);
}

TEST(GraphTest, print_double_features) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    AttributedGraph<double> graph(edges_path, features_path);

    // Redirect std::cout to capture the output
    std::ostringstream output;
    std::streambuf* oldCoutBuffer = std::cout.rdbuf(output.rdbuf());

    // Call the function to capture its output
    graph.print_features();

    // Restore std::cout
    std::cout.rdbuf(oldCoutBuffer);

    std::string expected_print =
        "0\t1, 0, #, 0, 0, 0\t2\n"
        "1\t0, 1, #, 0, 0, 0\t0\n"
        "2\t0, 0, #, 1, 0, 0\t1\n"
        "3\t0, 0, #, 0, 1, 0\t3\n"
        "4\t0, 0, #, 0, 0, 1\t4\n";

    EXPECT_EQ(output.str(), expected_print);
}

TEST(GraphTest, get_neighbours) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    AttributedGraph<double> graph(edges_path, features_path);

    std::vector<int> neighbours = graph.get_neighbours(0);
    std::vector<int> expected_neighbours = {1, 4};

    EXPECT_EQ(neighbours, expected_neighbours);

    // Test for a specific depth
    std::vector<int> depth_neighbours = graph.get_neighbours(0, 2);
    std::vector<int> expected_depth_neighbours = {3, 2, 4, 1, 0};

    EXPECT_EQ(depth_neighbours, expected_depth_neighbours);
}

TEST(GraphTest, get_degree) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    AttributedGraph<double> graph(edges_path, features_path);

    int degree = graph.get_degree(0);
    EXPECT_GE(degree, 2);
}

TEST(GraphTest, has_edge) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    AttributedGraph<double> graph(edges_path, features_path);

    EXPECT_TRUE(graph.has_edge(0, 4));   // Assumes edge exists
    EXPECT_FALSE(graph.has_edge(0, 2));  // Assumes edge does not exist
}

TEST(GraphTest, is_valid_node) {
    std::string edges_path = "../data/test/graph/edges_example.txt";
    std::string features_path = "../data/test/graph/features_example.txt";
    AttributedGraph<double> graph(edges_path, features_path);

    EXPECT_TRUE(graph.is_valid_node(0));
    EXPECT_FALSE(graph.is_valid_node(graph.get_num_nodes()));
}

// --- Edge Weight Tests ---

// A minimal concrete Graph implementation for testing (because the Graph class is abstract atm).
class DummyGraph : public AttributedGraph<double> {
   public:
    DummyGraph(const std::vector<int>& offs, const std::vector<int>& edgs) {
        offsets = offs;
        edges = edgs;
        num_nodes = offs.size() - 1;
    }

    size_t lookup(int u, int v) const {
        if (u < 0 || u >= get_num_nodes()) throw std::out_of_range("Invalid node");
        for (size_t i = offsets[u]; i < offsets[u + 1]; ++i) {
            if (edges[i] == v) return i;
        }
        throw std::logic_error("Edge not found");
    }
};

// Test that the GraphEdgeWeights is constructed with the default weight 1.0.
TEST(GraphEdgeWeightsTest, DefaultWeights) {
    std::vector<int> offsets = {0, 2, 4, 6};
    // Graph with 3 nodes:
    // Node 0: edges[0..1] -> {1, 2}
    // Node 1: edges[2..3] -> {0, 2}
    // Node 2: edges[4..5] -> {0, 1}
    std::vector<int> edges = {1, 2, 0, 2, 0, 1};
    DummyGraph dummy(offsets, edges);
    GraphEdgeWeights edge_weights(dummy);

    for (size_t i = 0; i < static_cast<size_t>(dummy.get_num_edges()); ++i) {
        EXPECT_DOUBLE_EQ(edge_weights.edge_weights[i], 1.0);
    }
}

// Test immediate edge retrieval for a node.
TEST(GraphEdgeWeightsTest, GetEdgesImmediate) {
    std::vector<int> offsets = {0, 2, 4, 6};
    std::vector<int> edges = {1, 2, 0, 2, 0, 1};
    DummyGraph dummy(offsets, edges);
    GraphEdgeWeights edge_weights(dummy);

    // Immediate edges from node 0.
    auto immediateEdges = edge_weights.get_edges(0);
    EXPECT_EQ(immediateEdges.size(), 2u);
    // Check that the targets are correct.
    EXPECT_EQ(immediateEdges[0].target, 1);
    EXPECT_EQ(immediateEdges[1].target, 2);
    // And weights should be default 1.0.
    EXPECT_DOUBLE_EQ(immediateEdges[0].weight, 1.0);
    EXPECT_DOUBLE_EQ(immediateEdges[1].weight, 1.0);
}

// Test edge retrieval with a given BFS depth.
TEST(GraphEdgeWeightsTest, GetEdgesWithDepth) {
    std::vector<int> offsets = {0, 2, 4, 6};
    std::vector<int> edges = {1, 2, 0, 2, 0, 1};
    DummyGraph dummy(offsets, edges);
    GraphEdgeWeights edge_weights(dummy);

    // For node 0 with depth = 1, should return immediate edges.
    auto depth1Edges = edge_weights.get_edges(0, 1);
    EXPECT_EQ(depth1Edges.size(), 2u);

    // For node 0 with depth = 2:
    // Depth 1: from node 0: edges (0->1) and (0->2).
    // Depth 2: from node 1: (1->0) and (1->2) and from node 2: (2->0) and (2->1).
    // Total edges: 2 + 2 + 2 = 6.
    auto depth2Edges = edge_weights.get_edges(0, 2);
    EXPECT_EQ(depth2Edges.size(), 6u);

    // Verify that the collected targets appear as expected:
    // Expected targets:
    //  From node 0: {1, 2}
    //  From node 1: {0, 2}
    //  From node 2: {0, 1}
    std::unordered_multiset<int> targets;
    for (const auto& edge : depth2Edges) {
        targets.insert(edge.target);
    }
    EXPECT_EQ(targets.count(0), 2);
    EXPECT_EQ(targets.count(1), 2);
    EXPECT_EQ(targets.count(2), 2);
}

// Test that an invalid node parameter throws an exception.
TEST(GraphEdgeWeightsTest, GetEdgesInvalidNode) {
    std::vector<int> offsets = {0, 2, 4, 6};
    std::vector<int> edges = {1, 2, 0, 2, 0, 1};
    DummyGraph dummy(offsets, edges);
    GraphEdgeWeights edge_weights(dummy);

    EXPECT_THROW(edge_weights.get_edges(-1), std::logic_error);
    EXPECT_THROW(edge_weights.get_edges(100), std::logic_error);
    EXPECT_THROW(edge_weights.get_edges(-1, 2), std::logic_error);
    EXPECT_THROW(edge_weights.get_edges(100, 2), std::logic_error);
}

TEST(GraphEdgeWeightsTest, IteratorFunctionality) {
    std::vector<int> offsets = {0, 2, 4, 6};
    std::vector<int> edges = {1, 2, 0, 2, 0, 1};
    DummyGraph dummy(offsets, edges);
    GraphEdgeWeights edge_weights(dummy);

    // Iterate through edges using the iterator.
    size_t count = 0;
    for (auto it = edge_weights.begin(); it != edge_weights.end(); ++it, ++count) {
        std::pair<int, int> edge = it.get_edge();
        // Determine expected source node based on CSR offsets:
        int expected_source = 0;
        if (count >= 2 && count < 4)
            expected_source = 1;
        else if (count >= 4)
            expected_source = 2;

        EXPECT_EQ(edge.first, expected_source);
        // The target should equal the corresponding entry in the edges vector.
        EXPECT_EQ(edge.second, edges[count]);
    }
    EXPECT_EQ(count, 6u);
}

TEST(GraphEdgeWeightsTest, ModifyWeightsViaIterator) {
    std::vector<int> offsets = {0, 2, 4, 6};
    std::vector<int> edges = {1, 2, 0, 2, 0, 1};
    DummyGraph dummy(offsets, edges);
    GraphEdgeWeights edge_weights(dummy);

    // Double each edge weight via the iterator.
    for (auto it = edge_weights.begin(); it != edge_weights.end(); ++it) {
        *it *= 2.0;
    }
    for (size_t i = 0; i < static_cast<size_t>(dummy.get_num_edges()); ++i) {
        EXPECT_DOUBLE_EQ(edge_weights.edge_weights[i], 2.0);
    }
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}