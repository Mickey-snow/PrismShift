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
  double uRoughness_, vRoughness_;

 public:
  ConductorMaterial(Color color, double uRough, double vRough);
  BSDF GetBSDF(const HitRecord& rec) const override;
};

class DielectricMaterial : public IMaterial {
  double eta_;
  double uRoughness_, vRoughness_;

 public:
  DielectricMaterial(double eta, double uRough, double vRough);
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
