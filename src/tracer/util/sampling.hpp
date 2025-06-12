#pragma once

#include "util/random.hpp"
#include "util/vector.hpp"

double pdf_cosine_distributed_hemisphere(const Vector3& wo);

Vector3 Spawn_cosine_distributed_hemisphere();

inline static Vector3 rand_hemisphere_uniform() {
  using std::numbers::pi;

  double u1 = random_uniform_01();
  double u2 = random_uniform_01();
  double phi = 2.0 * pi * u1;
  double cos_theta = u2;
  double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
  double x = sin_theta * std::cos(phi);
  double y = cos_theta;
  double z = sin_theta * std::sin(phi);
  return Vector3{x, y, z};
}
