#pragma once

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
    /**
     * @brief Constructs a matrix with a specified number of rows and columns.
     * @param num_rows Number of rows.
     * @param num_cols Number of columns.
     * @param value Initial value for each element (default 0.0).
     */
    Matrix(size_t num_rows, size_t num_cols, double value = 0.0);

    /**
     * @brief Returns the number of rows.
     * @return The row count.
     */
    size_t num_rows() const;

    /**
     * @brief Returns the number of columns.
     * @return The column count.
     */
    size_t num_cols() const;

    /**
     * @brief Provides mutable access to the element at the specified position.
     * @param row Row index.
     * @param col Column index.
     * @return Reference to the element.
     */
    double& operator()(size_t row, size_t col);

    /**
     * @brief Provides read-only access to the element at the specified position.
     * @param row Row index.
     * @param col Column index.
     * @return Const reference to the element.
     */
    const double& operator()(size_t row, size_t col) const;

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
