#include<cmath>

#include "color.hpp"
#include "interval.hpp"


inline double Linear2Gamma(const double& linear_component){
  return std::sqrt(linear_component);
}

Color Format_Color(const Color& pixel_color, const double& scale){
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  // Apply the linear to gamma transform
  r = Linear2Gamma(r);
  g = Linear2Gamma(g);
  b = Linear2Gamma(b);

  static const Interval intensity(0.000,0.999);
  return Color(scale * intensity.Clamp(r),
               scale * intensity.Clamp(g),
               scale * intensity.Clamp(b));
}
