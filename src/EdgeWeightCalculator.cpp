#include "EdgeWeightCalculator.hpp"
#include "WeightedGraph.hpp"

EdgeWeightCalculator::EdgeWeightCalculator(Graph& graph, double fusion_coefficient)
:graph(graph),fusion_coefficient(fusion_coefficient){
  
}


WeightedGraph EdgeWeightCalculator::generate_weighted_graph(){
    WeightedGraph wgraph(graph);
    
}

   
double compute_weight(int u, int v){

}
double compute_feature_similarity(int u, int v){

}
double compute_structural_similarity(int u, int v){

}