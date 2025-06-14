

#include "color.hpp"
#include "interval.hpp"

#include <cmath>

inline static double Linear2Gamma(double linear_component) {
  return std::pow(linear_component, 1.0 / 2.2);
}

Color Format_Color(const Color& pixel_color, const double& scale) {
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  // Apply the linear to gamma transform
  r = Linear2Gamma(r);
  g = Linear2Gamma(g);
  b = Linear2Gamma(b);

  static const Interval intensity(0.000, 0.999);
  return Color(scale * intensity.Clamp(r), scale * intensity.Clamp(g),
               scale * intensity.Clamp(b));
}
