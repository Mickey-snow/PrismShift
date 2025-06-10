#pragma once

#include <memory>
#include <util/color.hpp>

class BSDF;
class HitRecord;

class Material {
 public:
  ~Material() = default;

  virtual BSDF CalculateBSDF(const HitRecord&) = 0;

  virtual Color Emission(const HitRecord&) { return Color(0, 0, 0); }
};

class IMaterial {
 public:
  virtual ~IMaterial() = default;

  virtual BSDF CalcBSDF(const HitRecord&) const = 0;

  // Override if the material is emissive
  virtual Color Emission(const HitRecord& rec) { return Color(0, 0, 0); }
};
