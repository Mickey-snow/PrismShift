#pragma once

#include "util/util.hpp"

class TrowbridgeReitzDistribution {
 public:
  // Constructors
  TrowbridgeReitzDistribution() = default;
  TrowbridgeReitzDistribution(Float ax, Float az);

  // NDF evaluation
  Float D(Vector3 wm) const;

  // Visibility term
  Float G1(Vector3 w) const;
  Float G(Vector3 wi, Vector3 wo) const;

  // Masking-shadowing
  Float Lambda(Vector3 w) const;

  // Combined D·G·abs(dot) / cos term
  Float D(Vector3 w, Vector3 wm) const;
  Float PDF(Vector3 w, Vector3 wm) const;

  // Sampling
  Vector3 Sample_wm(Vector3 w) const;

  // Helpers
  static Float RoughnessToAlpha(Float roughness);
  bool EffectivelySmooth() const;
  void Regularize();

 private:
  Float alpha_x = 0.0, alpha_z = 0.0;
};
