#pragma once

#include "util/constant.hpp"
#include "util/random.hpp"
#include "util/vector.hpp"

double pdf_cosine_distributed_hemisphere(const Vector3& wo);

Vector3 Spawn_cosine_distributed_hemisphere();

inline static Vector3 rand_hemisphere_uniform() {
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

inline static Vector3 rand_sphere_uniform() {
  // u1 → controls z coordinate in [-1,1]
  // u2 → controls azimuth around the z-axis
  double u1 = random_uniform_01();
  double u2 = random_uniform_01();
  double z = 1.0 - 2.0 * u1;                         // Uniform in [-1,1]
  double r = std::sqrt(std::max(0.0, 1.0 - z * z));  // Radius in xy-plane
  double phi = 2.0 * pi * u2;                        // Uniform in [0,2pi)

  double x = r * std::cos(phi);
  double y = r * std::sin(phi);

  return Vector3{x, y, z};
}

inline Point2 SampleUniformDiskPolar() {
  double u0 = random_uniform_01(), u1 = random_uniform_01();
  double r = std::sqrt(u0);
  double theta = 2 * pi * u1;
  return Point2{r * std::cos(theta), r * std::sin(theta)};
}
