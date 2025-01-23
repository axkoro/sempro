#include "LouvainImputer.hpp"
#include "Graph.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <string>


TEST(LouvainImputerTest, TestFeatureImputation) {
    std::string edges_path = "../input/Gtests/edges_example.txt";
    std::string features_path = "../input/Gtests/features2_example.txt";
    Graph graph(edges_path, features_path);


    // Assigning nodes to two communities for testing purposes
    std::vector<int> communities = {0, 0, 0, 1, 1};

    LouvainImputer imputer(graph, communities);
    imputer.run();

    std::vector<std::vector<double>> expected_output = {
        {4.21, 3.97, 2.29, 1.78, 3.92, 2.69},
        {0.85, 2.515, 1.45, 2.54, 4.68, 4.37},
        {3.42, 1.06, 1.87, 2.62, 3.16, 1.78},
        {4.22, 2.14, 4.77, 1.24, 3.38, 4.39},
        {4.22, 0.38, 4.77, 2.81, 2.31, 2.98}
    };

    // Verify the output
    for (int node = 0; node < graph.get_num_nodes(); ++node) {
        for (int feature = 0; feature < graph.get_num_features() - 1; ++feature) {
            EXPECT_NEAR(graph.get_feature(node, feature), expected_output[node][feature], 1e-2)
                << "Node " << node << " Feature " << feature << " is incorrect.";
        }
    }

    graph.print_features();
}

TEST(LouvainImputerTest, TestFeatureImputationwithGlobalAverage) {
    std::string edges_path = "../input/Gtests/edges_example.txt";
    std::string features_path = "../input/Gtests/features3_example.txt";
    Graph graph(edges_path, features_path);

    std::vector<int> communities = {0, 0, 0, 1, 1};

    std::vector<std::vector<double>> expected_output = {
        {1, 0, 2.5, 0, 0, 0},
        {0, 1, 2.5, 0, 0, 0},
        {0, 0, 2.5, 1, 0, 0},
        {0, 0, 2, 0, 1, 0},
        {0, 0, 3, 0, 0, 1}
    };

    LouvainImputer imputer(graph, communities);
    imputer.run();

    for (int node = 0; node < graph.get_num_nodes(); ++node) {
        for (int feature = 0; feature < graph.get_num_features() - 1; ++feature) {
            EXPECT_NEAR(graph.get_feature(node, feature), expected_output[node][feature], 1e-2)
                << "Node " << node << " Feature " << feature << " is incorrect.";
        }
    }

    graph.print_features();


}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
