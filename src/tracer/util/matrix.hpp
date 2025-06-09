#pragma once

#include <cmath>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <optional>
#include <ostream>
#include <span>
#include <sstream>
#include <stdexcept>

#include "util/geometry.hpp"

template <std::size_t N, std::size_t M>
class Matrix {
 public:
  static constexpr auto ROWS = N;
  static constexpr auto COLUMNS = M;

  // data & data access
  std::array<std::array<double, COLUMNS>, ROWS> v;
  std::array<double, COLUMNS>& operator[](const std::size_t& idx) {
    return v[idx];
  }
  auto operator[](const std::size_t& idx) const -> decltype(auto) {
    return std::span<const double>(v[idx]);
  }

  Matrix() : v{} {}
  explicit Matrix(std::initializer_list<double> li) : Matrix() {
    auto it = li.begin();
    for (std::size_t i = 0; i < ROWS; ++i)
      for (std::size_t j = 0; j < COLUMNS; ++j) {
        if (it == li.end())
          break;
        v[i][j] = *it++;
      }
  }

  bool operator==(const Matrix<ROWS, COLUMNS>& rhs) const {
    static constexpr auto EPS = 1e-6;
    for (std::size_t i = 0; i < ROWS; ++i)
      for (std::size_t j = 0; j < COLUMNS; ++j)
        if (fabs(v[i][j] - rhs.v[i][j]) > EPS)
          return false;
    return true;
  }
  bool operator!=(const Matrix<ROWS, COLUMNS>& rhs) const {
    return !(*this == rhs);
  }

 public:
  /**
   * @brief Appends another matrixs to the right side of the current matrix.
   *
   * @return Matrix<ROWS, COLUMNS + K> - a new object that is the result of
   * appending 'mat' to the right side of the current matrix.
   */
  template <std::size_t K>
  auto AppendRight(const Matrix<ROWS, K>& mat) const -> decltype(auto) {
    using result_t = Matrix<ROWS, COLUMNS + K>;
    result_t res;
    for (std::size_t i = 0; i < ROWS; ++i) {
      std::copy(v[i].cbegin(), v[i].cend(), res.v[i].begin());
      std::copy(mat.v[i].cbegin(), mat.v[i].cend(), res.v[i].begin() + COLUMNS);
    }
    return res;
  }

  /**
   * @brief Appends another matrix of the same number of columns to the bottom
   * of the current matrix.
   *
   * @return Matrix<ROWS + K, COLUMNS> - a new object that is the result of
   * appending 'mat' to the bottom of the current matrix.
   */
  template <std::size_t K>
  auto AppendBottom(const Matrix<K, COLUMNS>& mat) const -> decltype(auto) {
    using result_t = Matrix<ROWS + K, COLUMNS>;
    result_t res;
    for (std::size_t i = 0; i < ROWS; ++i)
      std::copy(v[i].cbegin(), v[i].cend(), res.v[i].begin());
    for (std::size_t i = 0; i < K; ++i)
      std::copy(mat.v[i].cbegin(), mat.v[i].cend(), res.v[i + ROWS].begin());
    return res;
  }

  // row operations
  auto RowExchange(std::size_t row1, std::size_t row2) -> decltype(auto) {
    std::swap(v[row1], v[row2]);
    return *this;
  }
  auto RowAdd(std::size_t addee, std::size_t adder, double multiplier)
      -> decltype(auto) {
    for (std::size_t i = 0; i < COLUMNS; ++i)
      v[addee][i] += v[adder][i] * multiplier;
    return *this;
  }
  auto RowScale(std::size_t rowidx, double multiplier) -> decltype(auto) {
    for (std::size_t i = 0; i < COLUMNS; ++i)
      v[rowidx][i] *= multiplier;
    return *this;
  }

 public:
  template <std::size_t K>
  Matrix<ROWS, K> operator*(const Matrix<COLUMNS, K>& rhs) const {
    Matrix<ROWS, K> ans;
    for (std::size_t i = 0; i < ROWS; ++i)
      for (std::size_t j = 0; j < COLUMNS; ++j)
        for (std::size_t k = 0; k < K; ++k)
          ans.v[i][k] += v[i][j] * rhs.v[j][k];
    return ans;
  }
  template <std::size_t K>
  Matrix<ROWS, K>& operator*=(const Matrix<COLUMNS, K>& rhs) {
    return *this = *this * rhs;
  }
  template <vector_like T>
  auto operator*(const T& rhs) const -> decltype(auto)
    requires(T::dimension == COLUMNS)
  {
    using return_type =
        basic_vector<decltype(std::declval<double>() *
                              std::declval<typename T::value_type>()),
                     ROWS>;
    return_type ret;
    for (std::size_t i = 0; i < ROWS; ++i)
      for (std::size_t j = 0; j < COLUMNS; ++j)
        ret.v[i] += v[i][j] * rhs.v[j];
    return ret;
  }

 public:
  /**
   * @brief Transforms the matrix into its row-reduced echelon form (RREF).
   *
   * @return Matrix<ROWS, COLUMNS> The matrix in its row-reduced echelon form.
   */
  Matrix<ROWS, COLUMNS> rref() const { return rref_calculator(*this).doit(); }
  static auto rref(Matrix<ROWS, COLUMNS> mat) -> decltype(auto) {
    return rref_calculator(mat).doit();
  }

 private:  // helper class for rref()
  /**
   * @class rref_calculator
   * @brief A helper class for computing the Row-Reduced Echelon Form (RREF) of
   * a matrix.
   *
   * This class encapsulates the algorithm for transforming a matrix into its
   * row-reduced echelon form. It provides a high degree of customization,
   * allowing users to define their own behaviors for pivotal operations such as
   * row exchange, row scaling, row addition, and pivot searching. The class
   * implements a two-phase algorithm: a forward phase for converting the matrix
   * into an upper triangular form and a backward phase for normalizing pivot
   * rows and zeroing out elements above each pivot. The operations are
   * performed using customizable functors, enabling specialized behaviors like
   * counting row exchanges or skipping the scaling of pivot elements.
   *
   * Usage:
   *     Matrix<ROWS, COLUMNS> mat;
   *     rref_calculator rrefCalc(mat);
   *     Matrix<ROWS, COLUMNS> rrefMat = rrefCalc.doit();
   *
   * Customization:
   *     The class allows customization of the row reduction process by setting
   * different implementations for `rowexchange_fun`, `rowscale_fun`,
   * `rowadd_fun`, and `search_pivot_fun`. These are set to default
   * implementations in the constructor but can be overridden by the user.
   *
   * Example:
   *     rref_calculator rrefCalc(matrix);
   *     rrefCalc.rowexchange_fun = [](Matrix<ROWS, COLUMNS>& mat, std::size_t
   * row1, std::size_t row2){ ... }; rrefCalc.rowscale_fun = [](Matrix<ROWS,
   * COLUMNS>& mat, std::size_t rowidx, double scale){ ... }; Matrix<ROWS,
   * COLUMNS> rrefMatrix = rrefCalc.doit();
   */
  class rref_calculator {
   private:
    using Matrix_t = Matrix<ROWS, COLUMNS>;

   public:
    explicit rref_calculator(const Matrix_t& mat) : m_mat(mat) {
      rowexchange_fun = [](Matrix_t& mat, std::size_t row1, std::size_t row2) {
        mat.RowExchange(row1, row2);
      };
      rowscale_fun = [](Matrix_t& mat, std::size_t rowidx, double scale) {
        mat.RowScale(rowidx, scale);
      };
      rowadd_fun = [](Matrix_t& mat, std::size_t addeeRow, std::size_t adderRow,
                      double scale) { mat.RowAdd(addeeRow, adderRow, scale); };
      search_pivot_fun = [](const Matrix_t& mat, std::size_t colidx,
                            std::size_t toprow) {
        static constexpr auto EPS = 1e-8;
        for (std::size_t i = toprow; i < ROWS; ++i)
          if (fabs(mat.v[i][colidx]) > EPS)
            return std::optional<std::size_t>{i};
        return std::optional<std::size_t>{};
      };
    }

    Matrix_t& doit(void) {
      do_rref_fwd_phase();
      do_rref_bkwd_phase();
      return m_mat;
    }

    std::function<void(Matrix_t&, std::size_t, std::size_t)> rowexchange_fun;
    std::function<void(Matrix_t&, std::size_t, double)> rowscale_fun;
    std::function<void(Matrix_t&, std::size_t, std::size_t, double)> rowadd_fun;
    std::function<
        std::optional<std::size_t>(const Matrix_t&, std::size_t, std::size_t)>
        search_pivot_fun;

   private:
    // Forward phase: Transforms the matrix into an upper triangular form.
    // It iterates through each column, finds the first non-zero element
    // as the pivot, then zeroes out all elements below the pivot.
    void do_rref_fwd_phase() {
      std::size_t toprow = 0;
      for (std::size_t pivcol = 0; pivcol < COLUMNS; ++pivcol) {
        std::optional<std::size_t> pivrow =
            std::invoke(search_pivot_fun, m_mat, pivcol, toprow);
        // If no pivot is found in this column, continue to the next column
        if (!pivrow.has_value())
          continue;

        std::invoke(rowexchange_fun, m_mat, pivrow.value(), toprow);
        zero_out_pivcol(toprow + 1, ROWS, toprow, pivcol);

        ++toprow;
      }
    }
    // Backward phase: Normalizes each pivot row and zeroes out elements above
    // the pivot.
    void do_rref_bkwd_phase() {
      for (int i = ROWS - 1; i >= 0; --i) {
        static constexpr auto EPS = 1e-8;
        for (std::size_t j = 0; j < COLUMNS; ++j)
          if (fabs(m_mat.v[i][j]) > EPS) {
            std::invoke(rowscale_fun, m_mat, i, 1.0 / m_mat.v[i][j]);
            zero_out_pivcol(0, i, i, j);
            break;
          }
      }
    }
    // Zeroes out elements in a specified pivot column.
    // For each row in the specified range, it subtracts an appropriate multiple
    // of the pivot row from it, effectively setting the column element in that
    // row to zero.
    void zero_out_pivcol(std::size_t fr,
                         std::size_t to,
                         std::size_t pivrow,
                         std::size_t pivcol) {
      for (auto it = fr; it < to; ++it)
        std::invoke(rowadd_fun, m_mat, it, pivrow,
                    -m_mat.v[it][pivcol] / m_mat.v[pivrow][pivcol]);
    }
    Matrix_t m_mat;
  };

 public:
  /**
   * @brief Computes the transpose of the matrix.
   *
   * @return Matrix<COLUMNS, ROWS> - A new object which is the transpose of the
   * original matrix.
   *
   * Example:
   *   Matrix<3,2> mat; // Original matrix of size 3x2.
   *   auto transposedMat = mat.T(); // Transposed matrix of size 2x3.
   */
  Matrix<COLUMNS, ROWS> T(void) const {
    Matrix<COLUMNS, ROWS> ret;
    for (std::size_t i = 0; i < ROWS; ++i)
      for (std::size_t j = 0; j < COLUMNS; ++j)
        ret.v[j][i] = v[i][j];
    return ret;
  }

 public:
  /**
   * @brief Calculates the inverse of a square matrix.
   *
   * Computes the inverse of a square matrix using the
   * row-reduced echelon form (RREF) approach. It appends an identity matrix to
   * the right of the original matrix and then applies the RREF algorithm. The
   * right half of the resultant matrix is the inverse of the original matrix.
   *
   * @return Matrix<N,N> The inverse of the input matrix.
   *
   * @throws std::runtime_error if the matrix is not square or is singular.
   *
   * @example
   * Matrix<3,3> matrix; // Initialize matrix
   * Matrix<3,3> inverseMatrix = matrix.inv();
   */
  auto inv() const -> decltype(auto) {
    return Matrix<ROWS, COLUMNS>::inv(*this);
  }
  static auto inv(const Matrix<ROWS, COLUMNS>& mat) -> decltype(auto) {
    if constexpr (ROWS != COLUMNS) {
      throw std::runtime_error("Not a square matrix");
    } else {
      static constexpr auto K = ROWS;
      auto I_n = Matrix<K, K>::I();

      auto augmat = mat.AppendRight(I_n);
      augmat = augmat.rref();

      auto lside_is_In = [](const Matrix<K, K + K>& mat) {
        static constexpr auto EPS = 1e-8;
        for (std::size_t i = 0; i < K; ++i)
          for (std::size_t j = 0; j < K; ++j) {
            double expect = i == j ? 1 : 0;
            if (fabs(mat.v[i][j] - expect) > EPS)
              return false;
          }
        return true;
      };
      if (not lside_is_In(augmat))
        throw std::runtime_error("Singular matrix in MatirxInvert");

      Matrix<K, K> ans;
      for (std::size_t i = 0; i < K; ++i)
        for (std::size_t j = 0; j < K; ++j)
          ans.v[i][j] = augmat.v[i][j + K];
      return ans;
    }
  }

 public:
  /**
   * @brief Calculates the determinant of a square matrix.
   *
   * @return The determinant of the matrix.
   *
   * @throws std::runtime_error if the matrix is not square
   */
  double det() const { return Matrix<ROWS, COLUMNS>::det(*this); }
  static double det(const Matrix<ROWS, COLUMNS>& mat) {
    if constexpr (ROWS != COLUMNS)
      throw std::runtime_error("Not a square matrix");
    else {
      auto calculator = rref_calculator(mat);

      std::size_t row_exchange_count = 0;
      calculator.rowexchange_fun = [&row_exchange_count](
                                       Matrix<ROWS, COLUMNS>& mat,
                                       std::size_t row1, std::size_t row2) {
        if (row1 != row2) {
          ++row_exchange_count;  // keep a count on the row exchange performed
          mat.RowExchange(row1, row2);
        }
      };

      calculator.rowscale_fun = [](Matrix<ROWS, COLUMNS>&, std::size_t,
                                   double) {};
      // don't scale pivots to 1

      auto reduced = calculator.doit();
      double det = (row_exchange_count & 1) ? -1.0 : 1.0;
      for (std::size_t i = 0; i < ROWS; ++i)
        det *= reduced.v[i][i];
      return det;
    }
  }

 public:  // factory methods
  static Matrix<ROWS, COLUMNS> I(void) noexcept {
    Matrix<ROWS, COLUMNS> In;
    for (std::size_t i = 0; i < ROWS && i < COLUMNS; ++i)
      In.v[i][i] = 1;
    return In;
  }

 public:
  friend std::ostream& operator<<(std::ostream& os,
                                  const Matrix<ROWS, COLUMNS> m) {
    os << '(';
    for (std::size_t i = 0; i < ROWS; ++i) {
      os << '[';
      std::copy(m.v[i].cbegin(), m.v[i].cend(),
                std::ostream_iterator<double>(os, ","));
      os << "],";
    }
    os << ')';
    return os;
  }
};

using Matrix4 = Matrix<4, 4>;
