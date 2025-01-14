#pragma once

#include "GraphBool.hpp"
#include "GraphDouble.hpp"
#include "GraphInt.hpp"
#include "Imputer.hpp"

class KNNImputer : public Imputer {
   public:
    static constexpr int DEFAULT_K = 3;
    enum feature_type { b, d, i };  // bool, double, int

   private:
    int k = DEFAULT_K;
    feature_type type;

   public:
    explicit KNNImputer(GraphBool& g) : Imputer(g), type(b) {}
    explicit KNNImputer(GraphDouble& g) : Imputer(g), type(d) {}
    explicit KNNImputer(GraphInt& g) : Imputer(g), type(i) {}

    void run();

    // Configuration
    void set_depth(int k);

    // getter for k
    int get_depth();
};

double compute_global_average_bool(Graph& graph, int feature);
double compute_global_average_double(Graph& graph, int feature);
double compute_global_average_int(Graph& graph, int feature);

bool to_bool(double value);

int to_int(double value);