#pragma once

#include "util/util.hpp"

class TrowbridgeReitzDistribution {
 public:
  // Constructors
  TrowbridgeReitzDistribution() = default;
  TrowbridgeReitzDistribution(double ax, double az);

  // NDF evaluation
  double D(Vector3 wm) const;

  // Visibility term
  double G1(Vector3 w) const;
  double G(Vector3 wi, Vector3 wo) const;

  // Masking-shadowing
  double Lambda(Vector3 w) const;

  // Combined D·G·abs(dot) / cos term
  double D(Vector3 w, Vector3 wm) const;
  double PDF(Vector3 w, Vector3 wm) const;

  // Sampling
  Vector3 Sample_wm(Vector3 w) const;

  // Helpers
  static double RoughnessToAlpha(double roughness);
  bool EffectivelySmooth() const;
  void Regularize();

 private:
  double alpha_x = 0.0, alpha_z = 0.0;
};
