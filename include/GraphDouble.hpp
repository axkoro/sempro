#pragma once

#include "Graph.hpp"

class GraphDouble : public Graph {
   private:
    std::vector<std::vector<double>> features;

   public:
    using Graph::Graph;  // inherit constructors

    Feature get_feature(int node, int feature) const override;
    void set_feature(int node, int feature, Feature value) override;
    void read_features(std::string features_path) override;
    virtual void print_features() const override;
};