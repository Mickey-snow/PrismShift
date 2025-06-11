#pragma once

#include "util/vecmath.hpp"

class Color : public basic_vector<double, 3> {
 private:
  using super = basic_vector;

 public:
  using super::super;
  using value_type = typename super::value_type;
  static constexpr auto dimension = super::dimension;

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
