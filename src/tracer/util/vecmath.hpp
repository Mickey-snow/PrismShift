#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <type_traits>
#include <utility>

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <typename T>
concept vector_like = requires {
  typename std::remove_cvref_t<T>::value_type;
  std::remove_cvref_t<T>::dimension;
};

template <typename T, std::size_t N>
class basic_vector {
 public:
  using value_type = T;
  static constexpr std::size_t dimension = N;
  static constexpr auto EPS = 1e-6;

  // --- data ----------------------------------------------------------------
  std::array<T, N> v{};

  // --- ctors ---------------------------------------------------------------
  constexpr basic_vector() = default;

  constexpr basic_vector(std::initializer_list<T> li) {
    assert(li.size() == N && "wrong number of elements");
    std::copy(li.begin(), li.end(), v.begin());
  }
  template <typename... Args>
    requires(sizeof...(Args) == N && (std::convertible_to<Args, T> && ...))
  constexpr explicit basic_vector(Args&&... args)
      : v{static_cast<T>(args)...} {}
  constexpr explicit basic_vector(const std::array<T, N>& init) : v(init) {}

  // --- element access ------------------------------------------------------
  [[nodiscard]] constexpr T& operator[](std::size_t i) { return v[i]; }
  [[nodiscard]] constexpr const T& operator[](std::size_t i) const {
    return v[i];
  }

  // --- unary minus ---------------------------------------------------------
  [[nodiscard]] constexpr basic_vector operator-() const {
    basic_vector out;
    for (std::size_t i = 0; i < N; ++i)
      out.v[i] = -v[i];
    return out;
  }

 public:
  constexpr basic_vector operator+(const basic_vector& rhs) const {
    basic_vector ret;
    for (std::size_t i = 0; i < N; ++i)
      ret.v[i] = v[i] + rhs.v[i];
    return ret;
  }
  constexpr basic_vector operator-(const basic_vector& rhs) const {
    basic_vector ret;
    for (std::size_t i = 0; i < N; ++i)
      ret.v[i] = v[i] - rhs.v[i];
    return ret;
  }
  constexpr basic_vector operator*(const basic_vector& rhs) const {
    basic_vector ret;
    for (std::size_t i = 0; i < N; ++i)
      ret.v[i] = v[i] * rhs.v[i];
    return ret;
  }
  constexpr basic_vector operator/(const basic_vector& rhs) const {
    basic_vector ret;
    for (std::size_t i = 0; i < N; ++i)
      ret.v[i] = v[i] / rhs.v[i];
    return ret;
  }

  constexpr basic_vector operator*(arithmetic auto s) const {
    basic_vector ret;
    for (std::size_t i = 0; i < N; ++i)
      ret.v[i] = v[i] * s;
    return ret;
  }
  friend constexpr basic_vector operator*(arithmetic auto s, basic_vector a) {
    return a * s;
  }
  constexpr basic_vector operator/(arithmetic auto s) const {
    basic_vector ret;
    for (std::size_t i = 0; i < N; ++i)
      ret.v[i] = v[i] / s;
    return ret;
  }

  // --- dot / norm helpers -------------------------------------------------
  template <vector_like V, vector_like U>
  [[nodiscard]] static constexpr auto Dot(V const& lhs, U const& rhs)
    requires(V::dimension == U::dimension)
  {
    using R = decltype(std::declval<typename V::value_type>() *
                       std::declval<typename U::value_type>());
    R sum{};
    for (std::size_t i = 0; i < V::dimension; ++i)
      sum += lhs.v[i] * rhs.v[i];
    return sum;
  }
  [[nodiscard]] constexpr auto Dot(const basic_vector& rhs) const {
    return Dot(*this, rhs);
  }
  [[nodiscard]] constexpr T Length_squared() const { return Dot(*this); }
  [[nodiscard]] constexpr T Length() const {
    return std::sqrt(Length_squared());
  }

  [[nodiscard]] constexpr basic_vector normalised() const {
    return *this / Length();
  }

  [[nodiscard]] constexpr bool near_zero(T eps = static_cast<T>(1e-6)) const {
    for (auto e : v)
      if (std::fabs(e) > eps)
        return false;
    return true;
  }
  [[nodiscard]] inline constexpr bool operator==(
      const basic_vector& rhs) const {
    return (*this - rhs).near_zero();
  }
  [[nodiscard]] inline constexpr bool operator!=(
      const basic_vector& rhs) const {
    return !(*this == rhs);
  }

  constexpr value_type x() const { return v[0]; }
  constexpr value_type& x() { return v[0]; }
  constexpr value_type y() const { return v[1]; }
  constexpr value_type& y() { return v[1]; }
  constexpr value_type z() const { return v[2]; }
  constexpr value_type& z() { return v[2]; }
};

std::ostream& operator<<(std::ostream& os, const vector_like auto& it) {
  os << '{' << it.v[0];
  for (std::size_t i = 1; i < it.dimension; ++i)
    os << ',' << it.v[i];
  os << '}';
  return os;
}
