#ifndef MATERIAL_H
#define MATERIAL_H

#include "util/util.hpp"

class Material{
public:
  ~Material() = default;

  virtual Color Ray_Color(const Hit_record&) const = 0;
};


#endif
