#pragma once

#include "texture.hpp"
#include "util/color.hpp"

#include <memory>

class BSDF;
class HitRecord;

class IMaterial {
 public:
  virtual ~IMaterial() = default;

  virtual BSDF GetBSDF(const HitRecord& rec) const = 0;
};

class DiffuseMaterial : public IMaterial {
  Texture<Color> color;

 public:
  DiffuseMaterial(Color c);
  DiffuseMaterial(Texture<Color> c);

  BSDF GetBSDF(const HitRecord& rec) const override;
};

class ConductorMaterial : public IMaterial {
  Texture<Color> color_;
  Texture<double> uRoughness_, vRoughness_;

 public:
  ConductorMaterial(Color color, double uRough, double vRough);
  ConductorMaterial(Texture<Color> c, Texture<double> ur, Texture<double> vr);

  BSDF GetBSDF(const HitRecord& rec) const override;
};

class DielectricMaterial : public IMaterial {
  Texture<double> eta_, uRoughness_, vRoughness_;

 public:
  DielectricMaterial(double eta, double uRough, double vRough);
  DielectricMaterial(Texture<double> eta,
                     Texture<double> uRough,
                     Texture<double> vRough);

  BSDF GetBSDF(const HitRecord& rec) const override;
};

class MixedMaterial : public IMaterial {
  std::shared_ptr<IMaterial> first_, second_;
  double fac_;  // at 0, use first_ entirely

 public:
  MixedMaterial(std::shared_ptr<IMaterial> first,
                std::shared_ptr<IMaterial> second,
                double fac);

  std::shared_ptr<IMaterial> Select() const;

  // should not be called.
  BSDF GetBSDF(const HitRecord& rec) const override;
};
