#include "trowbridge_reitz_distribution.hpp"

#include <algorithm>
#include <cmath>

using namespace vec_helpers;

TrowbridgeReitzDistribution::TrowbridgeReitzDistribution(Float ax, Float az)
    : alpha_x(ax), alpha_z(az) {
  if (!EffectivelySmooth()) {
    alpha_x = std::max<Float>(alpha_x, 1e-4);
    alpha_z = std::max<Float>(alpha_z, 1e-4);
  }
}

Float TrowbridgeReitzDistribution::D(Vector3 wm) const {
  Float tan2Theta = Tan2Theta(wm);
  if (std::isinf(tan2Theta))
    return 0;
  Float cos4Theta = Sqr(Cos2Theta(wm));
  if (cos4Theta < 1e-16)
    return 0;
  Float e =
      tan2Theta * (Sqr(CosPhi(wm) / alpha_z) + Sqr(SinPhi(wm) / alpha_x));
  return 1.0 / (pi * alpha_z * alpha_x * cos4Theta * Sqr(1 + e));
}

bool TrowbridgeReitzDistribution::EffectivelySmooth() const {
  return alpha_z < 1e-3 && alpha_x < 1e-3;
}

Float TrowbridgeReitzDistribution::G1(Vector3 w) const {
  return 1.0 / (1.0 + Lambda(w));
}

Float TrowbridgeReitzDistribution::Lambda(Vector3 w) const {
  Float tan2Theta = Tan2Theta(w);
  if (std::isinf(tan2Theta))
    return 0;
  Float alpha2 = Sqr(CosPhi(w) * alpha_z) + Sqr(SinPhi(w) * alpha_x);
  return (std::sqrt(1 + alpha2 * tan2Theta) - 1) / 2;
}

Float TrowbridgeReitzDistribution::G(Vector3 wi, Vector3 wo) const {
  wi = -wi;
  return 1.0 / (1.0 + Lambda(wi) + Lambda(wo));
}

Float TrowbridgeReitzDistribution::D(Vector3 w, Vector3 wm) const {
  return G1(w) / AbsCosTheta(w) * D(wm) * std::abs(w.Dot(wm));
}

Float TrowbridgeReitzDistribution::PDF(Vector3 w, Vector3 wm) const {
  return D(w, wm);
}

Vector3 TrowbridgeReitzDistribution::Sample_wm(Vector3 w) const {
  // Hemispherical to ellipsoid transform
  Vector3 wh(alpha_x * w.x(), w.y(), alpha_z * w.z());
  wh = wh.Normalized();
  if (wh.y() < 0)
    wh = -wh;

  // Build orthonormal basis
  Vector3 T1 = (wh.y() < 0.99999)
                   ? Vector3::Cross(Vector3(0, 1, 0), wh).Normalized()
                   : Vector3(0, 0, 1);
  Vector3 T2 = Vector3::Cross(wh, T1);

  // Sample disk
  Point2 p = SampleUniformDiskPolar();
  Float h = std::sqrt(1 - Sqr(p.x()));
  p.y() = std::lerp(h, p.y(), (1 + wh.y()) / 2);

  // Project back to hemisphere
  Float py = std::sqrt(std::max(0.0, 1 - Vector2(p).Length_squared()));
  Vector3 nh = p.x() * T1 + p.y() * T2 + py * wh;

  // Ellipsoid to hemisphere
  Vector3 ans(alpha_x * nh.x(), std::max(1e-6, nh.y()), alpha_z * nh.z());
  return ans.Normalized();
}

Float TrowbridgeReitzDistribution::RoughnessToAlpha(Float roughness) {
  return std::sqrt(roughness);
}

void TrowbridgeReitzDistribution::Regularize() {
  if (alpha_x < 0.3)
    alpha_x = std::clamp<Float>(2 * alpha_x, 0.1, 0.3);
  if (alpha_z < 0.3)
    alpha_z = std::clamp<Float>(2 * alpha_z, 0.1, 0.3);
}
