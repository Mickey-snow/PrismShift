#ifndef MATERIAL_H
#define MATERIAL_H

#include "util/util.hpp"

class BxDF;
class Material{
public:
  ~Material() = default;

  virtual std::vector<std::shared_ptr<BxDF>> CalculateBSDF(const Hit_record&) = 0;

  virtual Color Emission(const Hit_record&){ return Color(0,0,0); }
};


#endif
