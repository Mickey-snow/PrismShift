#pragma once

#include "util/vecmath.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <utility>

template <std::size_t R, std::size_t C, typename value_type = double>
class Matrix {
  static_assert(R > 0 && C > 0, "Matrix must have non‑zero dimensions");

 public:
  static constexpr std::size_t rows = R;
  static constexpr std::size_t cols = C;
  static constexpr value_type eps = static_cast<value_type>(1e-12);

  using storage_type = std::array<value_type, R * C>;
  storage_type data_{};

  [[nodiscard]] static inline constexpr std::size_t idx(
      std::size_t r,
      std::size_t c) noexcept {
    return r * C + c;
  }

 public: /* -------------------------------- ctors --- */
  constexpr Matrix() noexcept = default;

  /* row‑major initialiser list: `{ a00, a01, … }` */
  constexpr explicit Matrix(std::initializer_list<value_type> init) noexcept {
    assert(init.size() <= data_.size());
    std::copy_n(init.begin(), init.size(), data_.begin());
  }

 public: /* ------------------------- access --- */
  /* pointer to the first element of the row (fast, cache‑friendly) */
  [[nodiscard]] constexpr value_type* operator[](std::size_t r) noexcept {
    return data_.data() + r * C;
  }
  [[nodiscard]] constexpr const value_type* operator[](
      std::size_t r) const noexcept {
    return data_.data() + r * C;
  }

  [[nodiscard]] constexpr value_type& at(std::size_t r,
                                         std::size_t c) noexcept {
    return data_[idx(r, c)];
  }
  [[nodiscard]] constexpr const value_type& at(std::size_t r,
                                               std::size_t c) const noexcept {
    return data_[idx(r, c)];
  }

 public: /* ------------------------ comparison --- */
  [[nodiscard]] constexpr bool approx_equal(
      const Matrix& rhs,
      value_type tolerance = eps) const noexcept {
    for (std::size_t i = 0; i < data_.size(); ++i)
      if (std::abs(data_[i] - rhs.data_[i]) > tolerance)
        return false;
    return true;
  }
  friend constexpr bool operator==(const Matrix& a, const Matrix& b) noexcept {
    return a.approx_equal(b);
  }
  friend constexpr bool operator!=(const Matrix& a, const Matrix& b) noexcept {
    return !(a == b);
  }

 public: /* ------------------------ factories --- */
  [[nodiscard]] static consteval Matrix I() noexcept
    requires(R == C)
  {
    Matrix m;
    for (std::size_t i = 0; i < R; ++i)
      m.at(i, i) = value_type{1};
    return m;
  }

 public: /* ------------------------ basic ops --- */
  /* fast row operations - used by rref/det/inv */
  constexpr void swap_rows(std::size_t r1, std::size_t r2) noexcept {
    if (r1 == r2)
      return;
    for (std::size_t c = 0; c < C; ++c)
      std::swap(at(r1, c), at(r2, c));
  }
  constexpr void scale_row(std::size_t r, value_type s) noexcept {
    for (std::size_t c = 0; c < C; ++c)
      at(r, c) *= s;
  }
  constexpr void add_row(std::size_t dst,
                         std::size_t src,
                         value_type s) noexcept {
    for (std::size_t c = 0; c < C; ++c)
      at(dst, c) += at(src, c) * s;
  }

  /* transpose */
  [[nodiscard]] constexpr Matrix<C, R, value_type> T() const noexcept {
    Matrix<C, R, value_type> out;
    for (std::size_t r = 0; r < R; ++r)
      for (std::size_t c = 0; c < C; ++c)
        out.at(c, r) = at(r, c);
    return out;
  }

  /* matrix × matrix (naïve triple loop, sizes are compile‑time so it will be
   * unrolled/vectorised by the optimiser) */
  template <std::size_t K>
  [[nodiscard]] constexpr Matrix<R, K, value_type> operator*(
      const Matrix<C, K, value_type>& rhs) const noexcept {
    Matrix<R, K, value_type> out;
    for (std::size_t r = 0; r < R; ++r)
      for (std::size_t k = 0; k < K; ++k) {
        value_type acc{};
        for (std::size_t c = 0; c < C; ++c)
          acc += at(r, c) * rhs.at(c, k);
        out.at(r, k) = acc;
      }
    return out;
  }

  template <vector_like V>
    requires(V::dimension == C)
  [[nodiscard]] constexpr auto operator*(const V& v) const {
    using out_scalar = decltype(std::declval<value_type>() *
                                std::declval<typename V::value_type>());
    basic_vector<out_scalar, R> out{};
    for (std::size_t r = 0; r < R; ++r) {
      out_scalar acc{};
      for (std::size_t c = 0; c < C; ++c)
        acc += (*this)[r][c] * v.v[c];
      out.v[r] = acc;
    }
    return out;
  }
  [[nodiscard]] friend constexpr auto operator*(const vector_like auto& v,
                                                const Matrix& m) {
    return m * v;
  }

  template <std::size_t K>
  constexpr Matrix& operator*=(const Matrix<C, K, value_type>& rhs) noexcept {
    return *this = (*this * rhs);
  }

 public: /* ------------------- append / concat --- */
  template <std::size_t K>
  [[nodiscard]] constexpr Matrix<R, C + K, value_type> append_right(
      const Matrix<R, K, value_type>& m) const noexcept {
    Matrix<R, C + K, value_type> out;
    /* copy this */
    std::copy_n(data_.begin(), data_.size(), out.data_.begin());
    /* copy rhs */
    for (std::size_t r = 0; r < R; ++r)
      std::copy_n(m[r], K, out[r] + C);
    return out;
  }

  template <std::size_t K>
  [[nodiscard]] constexpr Matrix<R + K, C, value_type> append_bottom(
      const Matrix<K, C, value_type>& m) const noexcept {
    Matrix<R + K, C, value_type> out;
    /* copy top */
    std::copy_n(data_.begin(), data_.size(), out.data_.begin());
    /* copy bottom */
    std::copy_n(m.data_.begin(), m.data_.size(),
                out.data_.begin() + data_.size());
    return out;
  }

 public: /* ---------------------- RREF / inverse / det --- */
  [[nodiscard]] constexpr Matrix rref() const noexcept {
    Matrix tmp{*this};
    std::size_t lead = 0;
    for (std::size_t r = 0; r < R; ++r) {
      if (lead >= C)
        break;
      std::size_t i = r;
      while (std::abs(tmp.at(i, lead)) <= eps) {
        if (++i == R) {
          i = r;
          if (++lead == C)
            return tmp;
        }
      }
      tmp.swap_rows(i, r);
      const value_type pivot = tmp.at(r, lead);
      tmp.scale_row(r, value_type{1} / pivot);
      for (std::size_t j = 0; j < R; ++j) {
        if (j == r)
          continue;
        const value_type factor = tmp.at(j, lead);
        if (std::abs(factor) > eps)
          tmp.add_row(j, r, -factor);
      }
      ++lead;
    }
    return tmp;
  }

  /* determinant via Gaussian elimination (O(N^3)) */
  [[nodiscard]] constexpr value_type det() const noexcept
    requires(R == C)
  {
    Matrix tmp{*this};
    value_type det{1};
    for (std::size_t i = 0; i < R; ++i) {
      /* find pivot (partial pivoting for stability) */
      std::size_t pivot = i;
      for (std::size_t r = i + 1; r < R; ++r)
        if (std::abs(tmp.at(r, i)) > std::abs(tmp.at(pivot, i)))
          pivot = r;
      if (std::abs(tmp.at(pivot, i)) <= eps)
        return 0;  // singular
      if (pivot != i) {
        tmp.swap_rows(pivot, i);
        det = -det;  // swap flips sign
      }
      det *= tmp.at(i, i);
      const value_type inv_piv = value_type{1} / tmp.at(i, i);
      for (std::size_t c = i; c < C; ++c)
        tmp.at(i, c) *= inv_piv;
      for (std::size_t r = i + 1; r < R; ++r) {
        const value_type f = tmp.at(r, i);
        if (std::abs(f) <= eps)
          continue;
        for (std::size_t c = i; c < C; ++c)
          tmp.at(r, c) -= f * tmp.at(i, c);
      }
    }
    return det;
  }

  [[nodiscard]] constexpr Matrix inv() const noexcept
    requires(R == C)
  {
    /* augment with identity and run Gauss‑Jordan */
    constexpr std::size_t K = R;
    using Aug = Matrix<R, 2 * K, value_type>;
    Aug aug;
    /* left half = *this */
    for (std::size_t i = 0; i < R; ++i)
      for (std::size_t j = 0; j < C; ++j)
        aug.at(i, j) = at(i, j);
    /* right half = I */
    for (std::size_t i = 0; i < K; ++i)
      aug.at(i, i + C) = value_type{1};

    /* RREF in‑place on augmented matrix */
    std::size_t lead = 0;
    for (std::size_t r = 0; r < R; ++r) {
      if (lead >= 2 * K)
        break;
      std::size_t i = r;
      while (std::abs(aug.at(i, lead)) <= eps) {
        if (++i == R) {
          i = r;
          if (++lead == 2 * K)
            throw std::runtime_error("singular matrix");
        }
      }
      aug.swap_rows(i, r);
      const value_type pivot = aug.at(r, lead);
      aug.scale_row(r, value_type{1} / pivot);
      for (std::size_t j = 0; j < R; ++j) {
        if (j == r)
          continue;
        const value_type f = aug.at(j, lead);
        if (std::abs(f) > eps)
          aug.add_row(j, r, -f);
      }
      ++lead;
    }

    /* extract right half */
    Matrix<R, C, value_type> out;
    for (std::size_t i = 0; i < R; ++i)
      for (std::size_t j = 0; j < C; ++j)
        out.at(i, j) = aug.at(i, j + C);
    return out;
  }

 public: /* ------------------------ stream out --- */
  friend std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    os << '[';
    for (std::size_t r = 0; r < R; ++r) {
      os << '[';
      for (std::size_t c = 0; c < C; ++c) {
        os << m.at(r, c);
        if (c + 1 < C)
          os << ',';
      }
      os << ']';
      if (r + 1 < R)
        os << '\n';
    }
    os << ']';
    return os;
  }
};

/* convenient alias for 4×4 double matrix (graphics etc.) */
using Matrix4 = Matrix<4, 4, double>;
