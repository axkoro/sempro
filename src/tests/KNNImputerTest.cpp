#include <gtest/gtest.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "KNNImputer.hpp"

class KNNTest : public testing::Test {};

TEST(KNNTest, testKNN) {
    std::string edges_path = "../data/test/knn/test_edges.txt";
    std::string features_path = "../data/test/knn/test_missing_features.txt";
    std::string complete_path = "../data/test/knn/test_complete_features.txt";

    AttributedGraph<double> graph(edges_path, features_path);
    KNNImputer<double> knn(graph, 2, true);
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

    std::remove(temp_file_path.c_str());
}

TEST(KNNTest, testGlobalAverage) {
    std::string edges_path = "../data/test/knn/test_edges.txt";
    std::string features_path = "../data/test/knn/test_missing_features.txt";

    AttributedGraph<double> graph(edges_path, features_path);
    KNNImputer<double> knn(graph, 3, true);

    double average0 = knn.compute_global_average(0);
    double average1 = knn.compute_global_average(1);
    double average2 = knn.compute_global_average(2);

    EXPECT_EQ(average0, 0);
    EXPECT_EQ(average1, 1);
    EXPECT_EQ(average2, 2);

    knn.run();

    average0 = knn.compute_global_average(0);
    average1 = knn.compute_global_average(1);
    average2 = knn.compute_global_average(2);

    EXPECT_EQ(average0, 0);
    EXPECT_NEAR(average1, 1.08, 1e-5);
    EXPECT_EQ(average2, 2);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}