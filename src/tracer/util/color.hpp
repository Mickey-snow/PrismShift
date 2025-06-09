#pragma once

#include "util/vecmath.hpp"

class Color : public basic_vector<double, 3> {
 private:
  using super = basic_vector;

 public:
  using super::super;

  explicit constexpr Color(vector_like auto c) : super(std::move(c)) {}

  inline constexpr Color operator*(vector_like auto const& rhs) const {
    return Color(super::operator*(rhs));
  }
  inline constexpr Color& operator*=(vector_like auto const& rhs) {
    return *this = *this * rhs;
  }
  inline constexpr Color operator*(arithmetic auto rhs) const {
    return Color(super::operator*(rhs));
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
  inline constexpr Color operator/=(arithmetic auto rhs) const {
    return *this = *this / rhs;
  }
};

Color Format_Color(const Color& pixel_color, const double& scale = 255.0);
