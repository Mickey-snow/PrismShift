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
  Texture<Color> albedo_;

 public:
  DiffuseMaterial(Color albedo);
  DiffuseMaterial(Texture<Color> albedo);

  BSDF GetBSDF(const HitRecord& rec) const override;
};

class ConductorMaterial : public IMaterial {
  Texture<Color> albedo_;
  Texture<Float> uRoughness_, vRoughness_;

 public:
  ConductorMaterial(Color albedo, Float uRough, Float vRough);
  ConductorMaterial(Texture<Color> albedo,
                    Texture<Float> ur,
                    Texture<Float> vr);

  BSDF GetBSDF(const HitRecord& rec) const override;
};

class DielectricMaterial : public IMaterial {
  Texture<Float> eta_, uRoughness_, vRoughness_;

 public:
  DielectricMaterial(Float eta, Float uRough, Float vRough);
  DielectricMaterial(Texture<Float> eta,
                     Texture<Float> uRough,
                     Texture<Float> vRough);

  BSDF GetBSDF(const HitRecord& rec) const override;
};

class MixedMaterial : public IMaterial {
  std::shared_ptr<IMaterial> first_, second_;
  Float fac_;  // at 0, use first_ entirely

 public:
  MixedMaterial(std::shared_ptr<IMaterial> first,
                std::shared_ptr<IMaterial> second,
                Float fac);

  std::shared_ptr<IMaterial> Select() const;

  // should not be called.
  BSDF GetBSDF(const HitRecord& rec) const override;
};
