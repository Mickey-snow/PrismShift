#pragma once

#include <cassert>
#include <cmath>
#include <concepts>
#include <format>
#include <functional>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <utility>

#include "util/vecmath.hpp"
#include "util/vector_fwd.hpp"
#ifdef ENABLE_SIMD
#include "util/simd_vector.hpp"
#endif

class ITransformation;

template <std::size_t N>
class Vector : public basic_vector<Float, N> {
 private:
  using super = basic_vector<Float, N>;
  static constexpr auto EPS = super::EPS;

 public:
  using super::super;
  using value_type = super::value_type;
  static constexpr auto dimension = super::dimension;

  explicit Vector(vector_like auto it) : super(std::move(it)) {}

  static Vector Normalized(const Vector<N>& v) { return v / v.Length(); }
  Vector Normalized() const { return Vector<N>::Normalized(*this); }

  inline bool isUnit() const {
    return fabs(super::Length_squared() - 1.0) < EPS;
  }
  inline bool NearZero() const { return super::Length_squared() < EPS; }

  static constexpr auto Dot(vector_like auto const& lhs,
                            vector_like auto const& rhs) {
    return super::Dot(lhs, rhs);
  }
  inline constexpr auto Dot(vector_like auto const& rhs) const {
    return Dot(*this, rhs);
  }

  template <vector_like T, vector_like U>
  static Vector<3> Cross(const T& lhs, const U& rhs)
    requires(T::dimension == 3 && U::dimension == 3 && N == 3)
  {
    return Vector<3>(lhs.v[1] * rhs.v[2] - lhs.v[2] * rhs.v[1],
                     lhs.v[2] * rhs.v[0] - lhs.v[0] * rhs.v[2],
                     lhs.v[0] * rhs.v[1] - lhs.v[1] * rhs.v[0]);
  }
  template <vector_like T, vector_like U>
  static Vector<3> Cross(const T& lhs, const U& rhs)
    requires(T::dimension == 2 && U::dimension == 2 && N == 2)
  {
    return Vector<3>(0, 0, lhs.v[0] * rhs.v[1] - lhs.v[1] * rhs.v[0]);
  }
  Vector Cross(const vector_like auto& rhs) const { return Cross(*this, rhs); }

  inline constexpr Vector operator+(const Vector& rhs) const {
    return Vector(super::operator+(rhs));
  }
  inline constexpr Vector& operator+=(const Vector& rhs) {
    return *this = *this + rhs;
  }
  inline constexpr Vector operator-() const {
    return Vector(super::operator-());
  }
  inline constexpr Vector operator-(const Vector& rhs) const {
    return Vector(super::operator-(rhs));
  }
  inline constexpr Vector& operator-=(const Vector& rhs) {
    return *this = *this - rhs;
  }
  inline constexpr Vector operator*(arithmetic auto rhs) const {
    return Vector(super::operator*(rhs));
  }
  inline constexpr Vector& operator*=(arithmetic auto rhs) {
    return *this = *this * rhs;
  }
  friend inline constexpr auto operator*(arithmetic auto lhs,
                                         const Vector& rhs) {
    return rhs * lhs;
  }
  inline constexpr Vector operator/(arithmetic auto rhs) const {
    return Vector(super::operator/(rhs));
  }
  inline constexpr Vector& operator/=(arithmetic auto rhs) {
    return *this = *this / rhs;
  }

  static Vector<N> Random_Unit(void);
  static Vector<N> Random(const Float& min, const Float& max);

  Vector<N> Transform(const ITransformation&) const;
};

template <std::size_t N>
class Point : public basic_vector<Float, N> {
 private:
  using super = basic_vector<Float, N>;

 public:
  using super::super;
  using value_type = super::value_type;
  static constexpr auto dimension = super::dimension;

  explicit Point(vector_like auto it) : super(std::move(it)) {}

  Point<N> Transform(const ITransformation&) const;

  inline constexpr Point operator+(const Vector<N>& rhs) const {
    return Point(super::operator+(rhs));
  }
  inline constexpr Point& operator+=(const Vector<N>& rhs) {
    return *this = *this + rhs;
  }
  inline constexpr friend Point operator+(const Vector<N>& lhs,
                                          const Point& rhs) {
    return rhs + lhs;
  }
  inline constexpr Vector<N> operator-(const Point& rhs) const {
    return Vector<N>(super::operator-(rhs));
  }
  inline constexpr Point operator-(const Vector<N>& rhs) const {
    return Point(super::operator-(rhs));
  }
  inline constexpr Point& operator-=(const Vector<N>& rhs) {
    return *this = *this - rhs;
  }
};

class Normal : public basic_vector<Float, 3> {
 private:
  using super = basic_vector;

 public:
  using value_type = super::value_type;
  static constexpr auto dimension = super::dimension;

  template <typename... Ts>
  explicit constexpr Normal(Ts&&... param) : super(std::forward<Ts>(param)...) {
    Normalize();
  }
  explicit constexpr Normal(vector_like auto it) : super(std::move(it)) {
    Normalize();
  }

  Normal Transform(const ITransformation&) const;

  inline constexpr Normal operator+(const Vector3& rhs) const {
    return Normal(x() + rhs.x(), y() + rhs.y(), z() + rhs.z());
  }
  inline constexpr Normal operator-(const Vector3& rhs) const {
    return Normal(x() - rhs.x(), y() - rhs.y(), z() - rhs.z());
  }
  inline constexpr Normal operator-() const { return Normal(-x(), -y(), -z()); }

  inline constexpr bool operator==(vector_like auto const& rhs) const {
    return super::operator==(rhs);
  }

 private:
  void Normalize() {
    auto length = super::Length();
    for (auto& it : v)
      it /= length;
  }
};

namespace vec_helpers {

template <vector_like U, vector_like V>
  requires(U::dimension == V::dimension)
bool IsParallel(U const& a, V const& b) {
  for (std::size_t i = 0; i < a.dimension; ++i)
    if (b[i] != 0) {
      Float t = a[i] / b[i];
      return b * t == a;
    }
  return true;
}
template <vector_like U, vector_like V>
  requires(U::dimension == V::dimension)
inline bool IsPerpendicular(U const& a, V const& b) {
  return std::abs(a.Dot(b)) < U::EPS;
}
inline Float CosTheta(const Vector3& w) { return w.y(); }
inline Float AbsCosTheta(const Vector3& w) { return std::abs(w.y()); }
inline Float Cos2Theta(const Vector3& w) { return w.y() * w.y(); }
inline Float Sin2Theta(const Vector3& w) {
  return std::max<Float>(0, 1 - Cos2Theta(w));
}
inline Float SinTheta(const Vector3& w) { return std::sqrt(Sin2Theta(w)); }
inline Float Tan2Theta(const Vector3& w) {
  return Sin2Theta(w) / Cos2Theta(w);
}
inline Float CosPhi(const Vector3& w) {
  Float sintheta = SinTheta(w);
  return sintheta == 0 ? 1.0 : std::clamp<Float>(w.z() / sintheta, -1, 1);
}
inline Float SinPhi(const Vector3& w) {
  Float sintheta = SinTheta(w);
  return sintheta == 0 ? 0.0 : std::clamp<Float>(w.x() / sintheta, -1, 1);
}

inline static bool SameHemisphere(const Vector3& w, const Vector3& wp) {
  return w.y() * wp.y() <= 0.0;
}
}  // namespace vec_helpers
