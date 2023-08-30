#include<iostream>
#include<cmath>

#include "color.hpp"
#include "interval.hpp"

inline double Linear2Gamma(const double& linear_component){
  return std::sqrt(linear_component);
}

Color Format_Color(const Color& pixel_color){
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  // Apply the linear to gamma transform
  r = Linear2Gamma(r);
  g = Linear2Gamma(g);
  b = Linear2Gamma(b);

  static const Interval intensity(0.000,0.999);
  return Color(256 * intensity.Clamp(r),
               256 * intensity.Clamp(g),
               256 * intensity.Clamp(b));
}

void Write_Color(std::ostream& out, const Color& pixel_color){
  auto formatted_pixel_color = Format_Color(pixel_color);

  out << static_cast<int>(formatted_pixel_color.x()) << ' '
      << static_cast<int>(formatted_pixel_color.y()) << ' '
      << static_cast<int>(formatted_pixel_color.z()) << '\n';
}
