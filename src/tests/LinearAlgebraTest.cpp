#include <gtest/gtest.h>

#include <vector>

#include "Vector.hpp"

TEST(VectorTest, ConstructorWithSize) {
    size_t n = 5;
    double value = 3.14;
    Vector vec(n, value);
    EXPECT_EQ(vec.size(), n);
    for (size_t i = 0; i < n; ++i) {
        EXPECT_DOUBLE_EQ(vec[i], value);
    }
}

TEST(VectorTest, ConstructorWithStdVector) {
    std::vector<double> stdVec = {1.0, 2.0, 3.0, 4.0};
    Vector vec(stdVec);
    EXPECT_EQ(vec.size(), stdVec.size());
    for (size_t i = 0; i < stdVec.size(); ++i) {
        EXPECT_DOUBLE_EQ(vec[i], stdVec[i]);
    }
}

TEST(VectorTest, ElementAccessAndModification) {
    Vector vec(3, 0.0);
    vec[0] = 1.0;
    vec[1] = 2.0;
    vec[2] = 3.0;
    EXPECT_DOUBLE_EQ(vec[0], 1.0);
    EXPECT_DOUBLE_EQ(vec[1], 2.0);
    EXPECT_DOUBLE_EQ(vec[2], 3.0);
}

TEST(VectorTest, DotProduct) {
    Vector a({1.0, 2.0, 3.0});
    Vector b({4.0, 5.0, 6.0});
    // Dot product: 1*4 + 2*5 + 3*6 = 32
    EXPECT_DOUBLE_EQ(a * b, 32.0);
}

TEST(VectorTest, ScalarMultiplication_VectorTimesScalar) {
    Vector vec({1.0, 2.0, 3.0});
    double scalar = 2.0;
    Vector result = vec * scalar;
    std::vector<double> expected = {2.0, 4.0, 6.0};
    EXPECT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_DOUBLE_EQ(result[i], expected[i]);
    }
}

TEST(VectorTest, ScalarMultiplication_ScalarTimesVector) {
    Vector vec({1.0, 2.0, 3.0});
    double scalar = 2.0;
    Vector result = scalar * vec;
    std::vector<double> expected = {2.0, 4.0, 6.0};
    EXPECT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_DOUBLE_EQ(result[i], expected[i]);
    }
}

TEST(VectorTest, CompoundAddition) {
    Vector a({1.0, 2.0, 3.0});
    Vector b({4.0, 5.0, 6.0});
    a += b;
    std::vector<double> expected = {5.0, 7.0, 9.0};
    EXPECT_EQ(a.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_DOUBLE_EQ(a[i], expected[i]);
    }
}

TEST(VectorTest, CompoundSubtraction) {
    Vector a({4.0, 5.0, 6.0});
    Vector b({1.0, 2.0, 3.0});
    a -= b;
    std::vector<double> expected = {3.0, 3.0, 3.0};
    EXPECT_EQ(a.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_DOUBLE_EQ(a[i], expected[i]);
    }
}

TEST(VectorTest, VectorAddition) {
    Vector a({1.0, 2.0, 3.0});
    Vector b({4.0, 5.0, 6.0});
    Vector result = a + b;
    std::vector<double> expected = {5.0, 7.0, 9.0};
    EXPECT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_DOUBLE_EQ(result[i], expected[i]);
    }
}

TEST(VectorTest, VectorSubtraction) {
    Vector a({4.0, 5.0, 6.0});
    Vector b({1.0, 2.0, 3.0});
    Vector result = a - b;
    std::vector<double> expected = {3.0, 3.0, 3.0};
    EXPECT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_DOUBLE_EQ(result[i], expected[i]);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
