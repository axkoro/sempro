#pragma once

#include "AttributedGraph.hpp"

template <typename T>
class Imputer {
   protected:
    AttributedGraph<T>& graph;

   public:
    explicit Imputer(AttributedGraph<T>& g) : graph(g) {}

    virtual void run() = 0;

   protected:
    double compute_global_average(int feature);
};

template <typename T>
T round_value(double value);

#include "Imputer.tpp"