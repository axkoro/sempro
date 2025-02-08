#include <gtest/gtest.h>

#include <vector>

#include "Matrix.hpp"
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

TEST(MatrixTest, ConstructorAndDefaultValue) {
    size_t rows = 3;
    size_t cols = 4;
    double defaultValue = 5.0;
    Matrix m(rows, cols, defaultValue);
    EXPECT_EQ(m.num_rows(), rows);
    EXPECT_EQ(m.num_cols(), cols);
    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < cols; ++j) EXPECT_DOUBLE_EQ(m(i, j), defaultValue);
}

TEST(MatrixTest, ElementAccessAndModification) {
    Matrix m(2, 3, 0.0);
    m(0, 0) = 1.0;
    m(0, 1) = 2.0;
    m(0, 2) = 3.0;
    m(1, 0) = 4.0;
    m(1, 1) = 5.0;
    m(1, 2) = 6.0;

    EXPECT_DOUBLE_EQ(m(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(m(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(m(0, 2), 3.0);
    EXPECT_DOUBLE_EQ(m(1, 0), 4.0);
    EXPECT_DOUBLE_EQ(m(1, 1), 5.0);
    EXPECT_DOUBLE_EQ(m(1, 2), 6.0);
}

TEST(MatrixTest, GetRow) {
    Matrix m(3, 3, 0.0);
    // Fill matrix with values:
    // row0: 1, 2, 3
    // row1: 4, 5, 6
    // row2: 7, 8, 9
    double counter = 1.0;
    for (size_t i = 0; i < m.num_rows(); ++i) {
        for (size_t j = 0; j < m.num_cols(); ++j) {
            m(i, j) = counter++;
        }
    }
    Vector row1 = m.get_row(1);
    EXPECT_EQ(row1.size(), m.num_cols());
    EXPECT_DOUBLE_EQ(row1[0], 4.0);
    EXPECT_DOUBLE_EQ(row1[1], 5.0);
    EXPECT_DOUBLE_EQ(row1[2], 6.0);
}

TEST(MatrixTest, AddToRow) {
    // Initialize a 2x3 matrix filled with 1's.
    Matrix m(2, 3, 1.0);
    // Create a vector with values to add.
    Vector addVec({2.0, 3.0, 4.0});
    m.add_to_row(addVec, 0);

    // After adding, row0 should become: 1+2, 1+3, 1+4 => 3, 4, 5.
    Vector row0 = m.get_row(0);
    EXPECT_DOUBLE_EQ(row0[0], 3.0);
    EXPECT_DOUBLE_EQ(row0[1], 4.0);
    EXPECT_DOUBLE_EQ(row0[2], 5.0);

    // Row1 should remain unchanged.
    Vector row1 = m.get_row(1);
    EXPECT_DOUBLE_EQ(row1[0], 1.0);
    EXPECT_DOUBLE_EQ(row1[1], 1.0);
    EXPECT_DOUBLE_EQ(row1[2], 1.0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
