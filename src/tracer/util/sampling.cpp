#include "sampling.hpp"
#include <util/constant.hpp>
#include <util/random.hpp>
#include <util/vector.hpp>

double pdf_cosine_distributed_hemisphere(const Vector3& wo) {
  const double len_sq = wo.Length_squared();
  if (len_sq == 0)
    return invpi;  // treat as aligned with +Z
  if (wo.z() <= 0)
    return 0;
  double costheta = wo.z() / std::sqrt(len_sq);
  return costheta * invpi;
}

Vector3 Spawn_cosine_distributed_hemisphere() {
  double r1 = random_uniform_01(), r2 = random_uniform_01();
  double phi = r1 * 2 * pi;

  double dx = cos(phi) * std::sqrt(r2);
  double dz = sin(phi) * std::sqrt(r2);
  double dy = std::sqrt(1 - r2);

  return Vector3{dx, dy, dz};
}
