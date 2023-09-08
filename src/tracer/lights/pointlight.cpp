#include "pointlight.hpp"

#include<util/util.hpp>

Color Pointlight::Ray_Color(const Hit_record&) const {
  return color;
}
