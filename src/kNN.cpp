#include "kNN.hpp"
#include "Graph.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

kNN::kNN(Graph& graph, int k) : graph(graph), k(k) {
    runkNN();
}
   
void kNN::runkNN() {
    

    std::vector<std::vector<double>> features = graph.getFeatures();
    std::vector<std::vector<bool>> missing = graph.getMissing();
    int num_nodes = graph.get_num_nodes();
    int num_features = graph.get_num_features();
    
    //for each node in graph if feature is missing take neighbours depth k and take average of features
    for (int i = 0; i < num_nodes ; i++)
    {
        for (int j= 0; j < num_features; j++)
        if(missing[i][j] == true)
        {
            std::vector<int> neighbours = graph.getNeighbours(i, k);
            double sum = 0;
            int count = 0;
            for (int n : neighbours)
            {
                if(missing[n][j] == false)
                {
                    sum += features[n][j];
                    count++;
                }
            }
            if(count > 0)
            {
                features[i][j] = sum/count;
            }else{
                features[i][j] = 0; //TODO: take average of all the nodes that have the feature
            }

           
        }
    
    }
    


}