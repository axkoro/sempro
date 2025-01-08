#pragma once

#include "Graph.hpp"

class GraphDouble : public Graph {
   private:
    std::vector<std::vector<double>> features;

   public:
    /**
     * @brief Constructs a graph by reading edge and feature files.
     *
     * @param edges_path Path to the file containing edges.
     * @param features_path Path to the file containing features.
     */
    GraphDouble(std::string edges_path, std::string features_path);

    double get_double_feature(int node, int feature) const override;
    void set_double_feature(int node, int feature, double value) override;
    void read_features(std::string features_path) override;
    virtual void print_features() const override;
};