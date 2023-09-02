#ifndef MATERIAL_H
#define MATERIAL_H

#include "util/util.hpp"

class Hit_record;
class Material{
public:
  ~Material() = default;

  virtual Color Ray_Color(const Ray&, const Hit_record&) const = 0;
};


#endif
