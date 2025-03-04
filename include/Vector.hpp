#pragma once

#include <vector>

class Matrix;  // forward-definition for friend class

/**
 * @brief Simple implementation of a vector.
 *
 * This class encapsulates a one-dimensional array of doubles and provides
 * common operations such as element access, dot product, scalar multiplication,
 * and vector addition/subtraction.
 */
class Vector {
    friend class Matrix;

   public:
    /**
     * @brief Constructs a vector of a given size, initializing all elements.
     * @param size Number of data.
     * @param value Initial value for each element (default 0.0).
     */
    Vector(size_t size, double value = 0.0);

    /**
     * @brief Constructs a vector from an existing std::vector.
     * @param vec Standard vector used to initialize the Vector.
     */
    Vector(std::vector<double> vec);

    /**
     * @brief Constructs a vector from a range of iterators.
     * @tparam Iterator Type of the iterator.
     * @param first Iterator pointing to the beginning of the range.
     * @param last Iterator pointing to the end of the range.
     */
    template <typename Iterator>
    Vector(Iterator first, Iterator last) : data(first, last) {}

    std::vector<double>::iterator begin() { return data.begin(); }
    std::vector<double>::iterator end() { return data.end(); }

    std::vector<double>::const_iterator begin() const { return data.begin(); }
    std::vector<double>::const_iterator end() const { return data.end(); }

    /**
     * @brief Returns the number of elements in the vector.
     * @return The size of the vector.
     */
    size_t size() const { return data.size(); }

    /**
     * @brief Provides mutable access to the element at the specified index.
     * @param idx Index of the element.
     * @return Reference to the element.
     */
    double& operator[](size_t idx) { return data[idx]; }

    /**
     * @brief Provides read-only access to the element at the specified index.
     * @param idx Index of the element.
     * @return Const reference to the element.
     */
    const double& operator[](size_t idx) const { return data[idx]; }

    /**
     * @brief Computes the dot product of two vectors.
     * @param lhs Left-hand side vector.
     * @param rhs Right-hand side vector.
     * @return The dot product result.
     */
    friend double operator*(const Vector& lhs, const Vector& rhs);

    /**
     * @brief Multiplies a vector by a scalar.
     * @param vec The vector to multiply.
     * @param scalar The scalar multiplier.
     * @return A new Vector resulting from the multiplication.
     */
    friend Vector operator*(const Vector& vec, double scalar);

    /**
     * @brief Multiplies a scalar by a vector.
     * @param scalar The scalar multiplier.
     * @param vec The vector to multiply.
     * @return A new Vector resulting from the multiplication.
     */
    friend Vector operator*(double scalar, const Vector& vec);

    /**
     * @brief Adds two vectors.
     * @param a First vector.
     * @param b Second vector.
     * @return A new Vector representing the element-wise sum.
     */
    friend Vector operator+(const Vector& a, const Vector& b);

    /**
     * @brief Subtracts one vector from another.
     * @param a First vector.
     * @param b Second vector.
     * @return A new Vector representing the element-wise difference.
     */
    friend Vector operator-(const Vector& a, const Vector& b);

    /**
     * @brief Adds another vector to this vector.
     * @param other The vector to add.
     * @return Reference to this vector after addition.
     */
    Vector& operator+=(const Vector& other);

    /**
     * @brief Subtracts another vector from this vector.
     * @param other The vector to subtract.
     * @return Reference to this vector after subtraction.
     */
    Vector& operator-=(const Vector& other);

   private:
    std::vector<double> data;  ///< Container for vector data.
};