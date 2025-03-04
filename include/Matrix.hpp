#pragma once

#include <algorithm>
#include <random>
#include <vector>

#include "Vector.hpp"

/**
 * @brief Simple implementation of a matrix stored in row-major format.
 *
 * This class represents a 2D matrix and supports basic operations such as
 * element access, row retrieval, and adding a vector to a row.
 */
class Matrix {
   public:
    Matrix() = default;

    /**
     * @brief Constructs a matrix with a specified number of rows and columns.
     * @param num_rows Number of rows.
     * @param num_cols Number of columns.
     * @param value Initial value for each element (default 0.0).
     */
    Matrix(size_t num_rows, size_t num_cols, double value = 0.0);

    /**
     * @brief Constructs a Matrix with elements initialized by a given distribution.
     *
     * This constructor creates a matrix of size @p num_rows by @p num_cols. Each element is
     * initialized by invoking the provided distribution callable.
     *
     * @tparam Distribution A callable type that takes no arguments and returns a double (or a type
     *                      convertible to double).
     * @param num_rows The number of rows in the matrix.
     * @param num_cols The number of columns in the matrix.
     * @param dist A callable used to generate initial values for the matrix elements.
     */
    template <typename Distribution>
    Matrix(size_t num_rows, size_t num_cols, Distribution dist)
        : num_rows_(num_rows), num_cols_(num_cols), data(num_rows * num_cols) {
        std::generate(data.begin(), data.end(), dist);
    }

    /**
     * @brief Returns the number of rows.
     * @return The row count.
     */
    size_t num_rows() const { return num_rows_; }

    /**
     * @brief Returns the number of columns.
     * @return The column count.
     */
    size_t num_cols() const { return num_cols_; }

    /**
     * @brief Provides mutable access to the element at the specified position.
     * @param row Row index.
     * @param col Column index.
     * @return Reference to the element.
     */
    double& operator()(size_t row, size_t col) { return data[row * num_cols_ + col]; }

    /**
     * @brief Provides read-only access to the element at the specified position.
     * @param row Row index.
     * @param col Column index.
     * @return Const reference to the element.
     */
    const double& operator()(size_t row, size_t col) const { return data[row * num_cols_ + col]; }

    /**
     * @brief Retrieves a specified row as a Vector.
     * @param row_idx Index of the row to retrieve.
     * @return A Vector containing the row's elements.
     */
    Vector get_row(size_t row_idx) const;

    /**
     * @brief Adds a Vector to a specific row.
     *
     * The provided vector must have a size equal to the number of columns.
     *
     * @param vec Vector to add.
     * @param row_idx Index of the row to modify.
     */
    void add_to_row(const Vector& vec, size_t row_idx);

   private:
    size_t num_rows_;          ///< Number of rows.
    size_t num_cols_;          ///< Number of columns.
    std::vector<double> data;  ///< Storage for matrix elements in row-major order.
};
