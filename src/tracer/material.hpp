#pragma once

#include <memory>
#include <util/color.hpp>

class BSDF;
class Hit_record;

class Material {
 public:
  ~Material() = default;

  virtual BSDF CalculateBSDF(const Hit_record&) = 0;

  virtual Color Emission(const Hit_record&) { return Color(0, 0, 0); }
};

class IMaterial {
 public:
  virtual ~IMaterial() = default;

  virtual BSDF CalcBSDF(const Hit_record&) const = 0;
};
