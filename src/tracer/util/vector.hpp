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

class ITransformation;

template <std::size_t N>
class Vector : public basic_vector<double, N> {
 private:
  using super = basic_vector<double, N>;
  static constexpr auto EPS = super::EPS;

 public:
  using super::super;
  using value_type = super::value_type;
  static constexpr auto dimension = super::dimension;

  explicit Vector(vector_like auto it) : super(std::move(it)) {}

  static Vector Normalized(const Vector<N>& v) { return v / v.Length(); }
  Vector Normalized() const { return Vector<N>::Normalized(*this); }

  bool isUnit(void) const { return fabs(super::Length_squared() - 1.0) < EPS; }

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

  static bool isPerpendicular(const Vector<N>& a, const Vector<N>& b) {
    return fabs(Vector<N>::Dot(a, b)) < EPS;
  }

  static bool Same_Direction(const Vector<N>& a, const Vector<N>& b) {
    if (a.Near_Zero() || b.Near_Zero())
      return true;
    double t;
    for (std::size_t i = 0; i < dimension; ++i)
      if (b[i] != 0) {
        t = a[i] / b[i];
        break;
      }
    return b * t == a;
  }

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
  static Vector<N> Random(const double& min, const double& max);

  Vector<N> Transform(const ITransformation&) const;
};

template <std::size_t N>
class Point : public basic_vector<double, N> {
 private:
  using super = basic_vector<double, N>;

 public:
  using super::super;
  using value_type = super::value_type;
  static constexpr auto dimension = super::dimension;

  explicit Point(vector_like auto it) : super(std::move(it)) {}

  Point<N> Transform(const ITransformation&) const;

  inline constexpr Point operator+(const Vector<N>& rhs) const {
    return Point(super::operator+(rhs));
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
};

class Normal : public basic_vector<double, 3> {
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

  constexpr Normal operator+(const Vector3& rhs) const {
    return Normal(x() + rhs.x(), y() + rhs.y(), z() + rhs.z());
  }
  constexpr Normal operator-(const Vector3& rhs) const {
    return Normal(x() - rhs.x(), y() - rhs.y(), z() - rhs.z());
  }
  constexpr Normal operator-() const { return Normal(-x(), -y(), -z()); }

 private:
  void Normalize() {
    auto length = super::Length();
    for (auto& it : v)
      it /= length;
  }
};
