#pragma once

#include "util/vecmath.hpp"

class Color : public basic_vector<double, 3> {
 private:
  using super = basic_vector;

 public:
  using super::super;
  using value_type = typename super::value_type;
  static constexpr auto dimension = super::dimension;

  explicit constexpr Color(double c) : super(c, c, c) {}
  explicit constexpr Color(vector_like auto c) : super(std::move(c)) {}

  inline constexpr value_type r() const { return v[0]; }
  inline constexpr value_type& r() { return v[0]; }
  inline constexpr value_type g() const { return v[1]; }
  inline constexpr value_type& g() { return v[1]; }
  inline constexpr value_type b() const { return v[2]; }
  inline constexpr value_type& b() { return v[2]; }

  inline constexpr Color operator+(const Color& rhs) const {
    return Color(super::operator+(rhs));
  }
  inline constexpr Color& operator+=(const Color& rhs) {
    return *this = *this + rhs;
  }
  inline constexpr Color operator*(vector_like auto const& rhs) const {
    return Color(super::operator*(rhs));
  }
  inline constexpr Color& operator*=(vector_like auto const& rhs) {
    return *this = *this * rhs;
  }
  inline constexpr Color operator*(arithmetic auto rhs) const {
    return Color(super::operator*(rhs));
  }
  inline constexpr friend Color operator*(arithmetic auto lhs,
                                          const Color& rhs) {
    return rhs * lhs;
  }
  inline constexpr Color& operator*=(arithmetic auto rhs) {
    return *this = *this * rhs;
  }
  inline constexpr Color operator/(vector_like auto const& rhs) const {
    return Color(super::operator/(rhs));
  }
  inline constexpr Color& operator/=(vector_like auto const& rhs) {
    return *this = *this / rhs;
  }
  inline constexpr Color operator/(arithmetic auto rhs) {
    return Color(super::operator/(rhs));
  }
  inline constexpr Color& operator/=(arithmetic auto rhs) {
    return *this = *this / rhs;
  }

  static constexpr Color Lerp(const Color& c1, const Color& c2, double t) {
    return Color(std::lerp(c1.x(), c2.x(), t), std::lerp(c1.y(), c2.y(), t),
                 std::lerp(c1.z(), c2.z(), t));
  }
};

Color Format_Color(const Color& pixel_color, const double& scale = 255.0);
