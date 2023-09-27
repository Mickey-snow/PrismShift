#ifndef MATERIAL_H
#define MATERIAL_H

#include<util/color.hpp>
#include<memory>

class BSDF;
class Hit_record;

class Material{
public:
  ~Material() = default;

  virtual BSDF CalculateBSDF(const Hit_record&) = 0;

  virtual Color Emission(const Hit_record&){ return Color(0,0,0); }
};


#endif
