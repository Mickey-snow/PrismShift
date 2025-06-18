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

inline auto Sqr(auto x) { return x * x; }

/**
 * Perfect-mirror reflection.
 *
 *  r = v − 2 (v·n) n
 *
 * Both `v` and `n` are assumed to be *unit* vectors.
 */
inline Vector3 Reflect(const Vector3& wi, const Vector3& n) {
  return wi - 2.0 * wi.Dot(n) * n;
}

inline bool Refract(Vector3 v,
                    const Vector3& n,
                    double eta_ratio,
                    Vector3* out) {
  if (!out)
    return false;

  v = -v;
  const double cosThetaI = v.Dot(n);
  const double sin2ThetaI = std::max(0.0, 1.0 - cosThetaI * cosThetaI);
  const double sin2ThetaT = eta_ratio * eta_ratio * sin2ThetaI;

  if (sin2ThetaT >= 1.0)
    return false;

  const double cosThetaT = std::sqrt(std::max(0.0, 1.0 - sin2ThetaT));

  *out = eta_ratio * (-v) + (eta_ratio * cosThetaI - cosThetaT) * n;
  out->y() *= out->y() * v.y() < 0 ? 1 : -1;
  return true;
}
