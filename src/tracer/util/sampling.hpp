#pragma once

#include "util/constant.hpp"
#include "util/random.hpp"
#include "util/vector.hpp"

Float pdf_cosine_distributed_hemisphere(const Vector3& wo);

Vector3 Spawn_cosine_distributed_hemisphere();

inline static Vector3 rand_hemisphere_uniform() {
  Float u1 = random_uniform_01();
  Float u2 = random_uniform_01();
  Float phi = 2.0 * pi * u1;
  Float cos_theta = u2;
  Float sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
  Float x = sin_theta * std::cos(phi);
  Float y = cos_theta;
  Float z = sin_theta * std::sin(phi);
  return Vector3{x, y, z};
}

inline static Vector3 rand_sphere_uniform() {
  // u1 → controls z coordinate in [-1,1]
  // u2 → controls azimuth around the z-axis
  Float u1 = random_uniform_01();
  Float u2 = random_uniform_01();
  Float z = 1.0 - 2.0 * u1;                         // Uniform in [-1,1]
  Float r = std::sqrt(std::max(0.0, 1.0 - z * z));  // Radius in xy-plane
  Float phi = 2.0 * pi * u2;                        // Uniform in [0,2pi)

  Float x = r * std::cos(phi);
  Float y = r * std::sin(phi);

  return Vector3{x, y, z};
}

inline Point2 SampleUniformDiskPolar() {
  Float u0 = random_uniform_01(), u1 = random_uniform_01();
  Float r = std::sqrt(u0);
  Float theta = 2 * pi * u1;
  return Point2{r * std::cos(theta), r * std::sin(theta)};
}
