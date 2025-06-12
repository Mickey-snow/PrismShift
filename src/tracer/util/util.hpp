#pragma once

#include <cassert>

// Headers
#include "aabb.hpp"
#include "color.hpp"
#include "constant.hpp"
#include "interval.hpp"
#include "mat.hpp"
#include "matrix.hpp"
#include "random.hpp"
#include "ray.hpp"
#include "sampling.hpp"
#include "transform.hpp"
#include "vecmath.hpp"
#include "vector.hpp"

// Utility Functions
inline constexpr auto Degree2Rad(std::floating_point auto degrees) {
  return degrees * pi / 180.0;
}
inline constexpr auto Rad2Degree(std::floating_point auto rad) {
  return rad / 180.0 * pi;
}

template <typename... Ts>
struct overload : Ts... {
  overload(Ts... ts) : Ts(std::move(ts))... {}
  using Ts::operator()...;
};
template <typename... Ts>
overload(Ts...) -> overload<Ts...>;

/**
 * Perfect-mirror reflection.
 *
 *  r = v − 2 (v·n) n
 *
 * Both `v` and `n` are assumed to be *unit* vectors.
 */
inline Vector3 Reflect(const Vector3& v, const Vector3& n) {
  return v - 2.0 * v.Dot(n) * n;
}

/* Convenience overload for the tracer’s local shading frame
 * (normal is always +Y = (0, 1, 0)).  Saves a dot-product. */
inline Vector3 Reflect(const Vector3& v) { return {v.x(), -v.y(), v.z()}; }

/**
 * Snell refraction.
 *
 *  v          – incident unit vector *toward* the surface
 *  n          – unit surface normal pointing *outward*
 *  eta_ratio  – ηᵢ ⁄ ηₜ  (e.g. 1/η when entering, η/1 when exiting)
 *  out        – stores the refracted unit vector on success
 *
 * Returns *false* when total internal reflection occurs.
 */
inline bool Refract(const Vector3& v,
                    const Vector3& n,
                    double eta_ratio,
                    Vector3* out) {
  const double cosThetaI = v.Dot(n);  // ±cosθᵢ
  const double sin2ThetaI = std::max(0.0, 1.0 - cosThetaI * cosThetaI);
  const double sin2ThetaT = eta_ratio * eta_ratio * sin2ThetaI;

  /* Total-internal-reflection check */
  if (sin2ThetaT >= 1.0)
    return false;

  const double cosThetaT = std::sqrt(std::max(0.0, 1.0 - sin2ThetaT));

  /* Transmitted direction (unit length).  Sign conventions:
   *  -v  flips the incident vector to point *away* from the surface.
   */
  *out = eta_ratio * (-v) + (eta_ratio * cosThetaI - cosThetaT) * n;
  return true;
}
