#ifndef TEXTURE_H
#define TEXTURE_H

#include<util/util.hpp>

class Texture{
public:
  ~Texture() = default;

  virtual Color ColorAt(const Point2&) = 0;
};

#endif
