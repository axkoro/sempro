#include "LouvainImputer.hpp"
#include "KNNImputer.hpp"
#include <gtest/gtest.h>

TEST(LouvainImputerTest, TestFeatureImputationDouble) {
    std::string edges_path = "../data/test/louvain/edges_example.txt";
    std::string features_path = "../data/test/louvain/features2_example.txt";
    GraphDouble graph(edges_path, features_path);

    // Assigning nodes to two communities for testing purposes
    std::vector<int> communities = {0, 0, 0, 1, 1};

    LouvainImputer imputer(graph, communities);
    imputer.run();

    std::vector<std::vector<double>> expected_output = {{4.21, 3.97, 2.29, 1.78, 3.92, 2.69},
                                                        {0.85, 2.515, 1.45, 2.54, 4.68, 4.37},
                                                        {3.42, 1.06, 1.87, 2.62, 3.16, 1.78},
                                                        {4.22, 2.14, 4.77, 1.24, 3.38, 4.39},
                                                        {4.22, 0.38, 4.77, 2.81, 2.31, 2.98}};

    // Verify the output
    for (int node = 0; node < graph.get_num_nodes(); ++node) {
        for (int feature = 0; feature < graph.get_num_features() - 1; ++feature) {
            EXPECT_NEAR(graph.get_double_feature(node, feature), expected_output[node][feature], 1e-2)
                << "Node " << node << " Feature " << feature << " is incorrect.";
        }
    }
}

TEST(LouvainImputerTest, TestFeatureImputationBool) {
    std::string edges_path = "../data/test/louvain/edges_example.txt";
    std::string features_path = "../data/test/louvain/features_example.txt";
    GraphBool graph(edges_path, features_path);

    // Assigning nodes to two communities for testing purposes
    std::vector<int> communities = {0, 0, 0, 1, 1};

    LouvainImputer imputer(graph, communities);
    imputer.run();

    std::vector<std::vector<bool>> expected_output = {{true, false, true, true, false, true},
                                                      {false, false, true, false, false, false},
                                                      {true, false, true, true, false, true},
                                                      {false, true, false, false, true, false},
                                                      {false, true, true, false, false, true}};

    // Verify the output
    for (int node = 0; node < graph.get_num_nodes(); ++node) {
        for (int feature = 0; feature < graph.get_num_features() - 1; ++feature) {
            EXPECT_EQ(graph.get_bool_feature(node, feature), expected_output[node][feature])
                << "Node " << node << " Feature " << feature << " is incorrect.";
        }
    }
}

TEST(LouvainImputerTest, TestFeatureImputationInt) {
    std::string edges_path = "../data/test/louvain/edges_example.txt";
    std::string features_path = "../data/test/louvain/features2_example.txt";
    GraphInt graph(edges_path, features_path);

    // Assigning nodes to two communities for testing purposes
    std::vector<int> communities = {0, 0, 0, 1, 1};

    LouvainImputer imputer(graph, communities);
    imputer.run();

    std::vector<std::vector<int>> expected_output = {{4, 3, 2, 1, 3, 2},
                                                     {0, 2, 1, 2, 4, 4},
                                                     {3, 1, 1, 2, 3, 1},
                                                     {4, 2, 4, 1, 3, 4},
                                                     {4, 0, 4, 2, 2, 2}};

    // Verify the output
    for (int node = 0; node < graph.get_num_nodes(); ++node) {
        for (int feature = 0; feature < graph.get_num_features() - 1; ++feature) {
            EXPECT_EQ(graph.get_int_feature(node, feature), expected_output[node][feature])
                << "Node " << node << " Feature " << feature << " is incorrect.";
        }
    }
}
int main(int, char**) {
    ::testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}