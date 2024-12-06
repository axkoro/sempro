#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <sstream>  
#include "KNNImputer.hpp"
#include "Graph.hpp"

class KNNTest : public testing::Test {};
//test using input/test/test.txt`s data
TEST( KNNTest, testKNN){
    
    std::string edges_path = "../input/test/test_edges.txt";
    std::string features_path = "../input/test/test_missing_features.txt";
    std::string complete_path = "../input/test/test_complete_features.txt";
    Graph graph(edges_path, features_path);
    KNNImputer knn(graph);
    knn.set_depth(2);
    knn.run();
    std::cout << "Number of nodes: " << graph.get_num_nodes() << std::endl;

     std::string temp_file_path = "temp_edges.txt";
    {
        std::ofstream output(temp_file_path);
        // Redirect cout to the file
        std::streambuf* old = std::cout.rdbuf(output.rdbuf());
        graph.print_features();
        std::cout.rdbuf(old);
        output.close();
    }
    graph.print_features();

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
    std::remove(temp_file_path.c_str());
    
}
int main(int, char**) {
    ::testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}