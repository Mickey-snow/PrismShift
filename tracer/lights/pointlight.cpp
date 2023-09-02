#include "pointlight.hpp"

#include<util/util.hpp>

Color Pointlight::Ray_Color(const Ray& r, const Hit_record&) const {
  return color;
}
