#pragma once

#include "Graph.hpp"

class GraphInt : public Graph {
   private:
    std::vector<std::vector<int>> features;

   public:
    /**
     * @brief Constructs a graph by reading edge and feature files.
     *
     * @param edges_path Path to the file containing edges.
     * @param features_path Path to the file containing features.
     */
    GraphInt(std::string edges_path, std::string features_path);

    int get_int_feature(int node, int feature) const override;
    void set_int_feature(int node, int feature, int value) override;
    void read_features(std::string features_path) override;
    virtual void print_features() const override;
    virtual void print_features_to_file(std::string features_path) const override;
};