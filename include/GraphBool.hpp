#pragma once

#include "Graph.hpp"

class GraphBool : public Graph {
   private:
    std::vector<std::vector<bool>> features;

   public:
    /**
     * @brief Constructs a graph by reading edge and feature files.
     *
     * @param edges_path Path to the file containing edges.
     * @param features_path Path to the file containing features.
     */
    GraphBool(std::string edges_path, std::string features_path);

    bool get_bool_feature(int node, int feature) const override;
    void set_bool_feature(int node, int feature, bool value) override;
    void read_features(std::string features_path) override;
    virtual void print_features() const override;
};