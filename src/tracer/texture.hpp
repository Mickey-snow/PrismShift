#pragma once

#include "util/color.hpp"
#include "util/geometry_fwd.hpp"

class Texture {
 public:
  ~Texture() = default;

  virtual Color ColorAt(const Point2&) = 0;
};
