#ifndef TEXTURE_H
#define TEXTURE_H

#include "util/geometry_fwd.hpp"
#include "util/color.hpp"

class Texture{
public:
  ~Texture() = default;

  virtual Color ColorAt(const Point2&) = 0;
};

#endif
