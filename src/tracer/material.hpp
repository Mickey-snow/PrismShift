#pragma once

#include <memory>
#include <util/color.hpp>

class BSDF;
class HitRecord;
template <typename T>
class Texture;

class IMaterial {
 public:
  virtual ~IMaterial() = default;

  virtual BSDF GetBSDF(const HitRecord& rec) const = 0;
};

class DiffuseMaterial : public IMaterial {
  Color color;

 public:
  DiffuseMaterial(Color c);
  BSDF GetBSDF(const HitRecord& rec) const override;
};

class ConductorMaterial : public IMaterial {
  Color color_;
  double fuzz_;
  double uRoughness_ = 0.99, vRoughness_ = 0.98;

 public:
  ConductorMaterial(Color color, double fuzz);
  BSDF GetBSDF(const HitRecord& rec) const override;
};

class DielectricMaterial : public IMaterial {
  double eta_;
  double uRoughness_ = 0.5, vRoughness_ = 0.5;

 public:
  DielectricMaterial(double eta);
  BSDF GetBSDF(const HitRecord& rec) const override;
};
