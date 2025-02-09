#include <gtest/gtest.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "Graph.hpp"
#include "KNNImputer.hpp"

class KNNTest : public testing::Test {};

TEST(KNNTest, testKNN) {
    std::string edges_path = "../data/test/knn/test_edges.txt";
    std::string features_path = "../data/test/knn/test_missing_features.txt";
    std::string complete_path = "../data/test/knn/test_complete_features.txt";
    GraphDouble graph(edges_path, features_path);
    KNNImputer knn(graph);
    knn.set_depth(2);
    knn.run();

    std::string temp_file_path = "temp_imputation_results.txt";
    {
        std::ofstream output(temp_file_path);
        // Redirect cout to the file
        std::streambuf* old = std::cout.rdbuf(output.rdbuf());
        graph.print_features();
        std::cout.rdbuf(old);
        output.close();
    }

    // Compare files line by line
    std::ifstream file1(complete_path);
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
    // std::remove(temp_file_path.c_str());
}

TEST(KNNTest, testDepth) {
    std::string edges_path = "../data/test/knn/test_edges.txt";
    std::string features_path = "../data/test/knn/test_missing_features.txt";
    GraphDouble graph(edges_path, features_path);
    KNNImputer knn(graph);
    knn.set_depth(3);
    EXPECT_EQ(knn.get_depth(), 3);
    EXPECT_NE(knn.get_depth(), 2);
    knn.set_depth(5);
    EXPECT_EQ(knn.get_depth(), 5);
    EXPECT_NE(knn.get_depth(), 3);
}
TEST(KNNTest, testGlobalAverage) {
    std::setprecision(5);
    std::string edges_path = "../data/test/knn/test_edges.txt";
    std::string features_path = "../data/test/knn/test_missing_features.txt";
    GraphDouble graph(edges_path, features_path);
    KNNImputer knn = KNNImputer(graph);
    double average0 = compute_global_average_double(graph, 0);
    double average1 = compute_global_average_double(graph, 1);
    double average2 = compute_global_average_double(graph, 2);

    EXPECT_EQ(average0, 0);
    EXPECT_EQ(average1, 1);
    EXPECT_EQ(average2, 2);

    knn.set_depth(3);
    knn.run();

    average0 = compute_global_average_double(graph, 0);
    average1 = compute_global_average_double(graph, 1);
    average2 = compute_global_average_double(graph, 2);

    EXPECT_EQ(average0, 0);
    EXPECT_NEAR(average1, 1.08, 1e-5);
    EXPECT_EQ(average2, 2);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}