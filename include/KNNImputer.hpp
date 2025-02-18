#pragma once

#include "Imputer.hpp"

#define FRIEND_TEST(test_case_name, test_name) friend class test_case_name##_##test_name##_Test

template <typename T>
class KNNImputer : public Imputer<T> {
    FRIEND_TEST(KNNTest, testGlobalAverage);

   private:
    int depth;
    bool use_k_nearest;

   public:
    KNNImputer(AttributedGraph<T>& g, int depth = 2, bool use_k_nearest = false)
        : Imputer<T>(g), depth(depth), use_k_nearest(use_k_nearest) {}

    void run() override;
};

#include "KNNImputer.tpp"