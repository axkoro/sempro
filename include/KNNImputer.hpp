#pragma once

#include "GraphBool.hpp"
#include "GraphDouble.hpp"
#include "GraphInt.hpp"
#include "Imputer.hpp"

class KNNImputer : public Imputer {
   public:
    static constexpr int DEFAULT_K = 3;

   private:
    int k = DEFAULT_K;
    Graph::feature_type type;

   public:
    explicit KNNImputer(GraphBool& g) : Imputer(g), type(Graph::feature_type::b) {}
    explicit KNNImputer(GraphDouble& g) : Imputer(g), type(Graph::feature_type::d) {}
    explicit KNNImputer(GraphInt& g) : Imputer(g), type(Graph::feature_type::i) {}

    void run();

    // Configuration
    void set_depth(int k);

    // getter for k
    int get_depth();
};