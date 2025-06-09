#include "sampling.hpp"
#include <util/constant.hpp>
#include <util/geometry.hpp>
#include <util/random.hpp>

double pdf_cosine_distributed_hemisphere(const Vector3& wo) {
  if (wo.z() <= 0)
    return 0;
  double costheta = wo.z() / wo.Length();
  return costheta * invpi;
}

Vector3 Spawn_cosine_distributed_hemisphere() {
  double r1 = random_uniform_01(), r2 = random_uniform_01();

  double phi = r1 * 2 * pi;
  double x = cos(phi) * std::sqrt(r2);
  double y = sin(phi) * std::sqrt(r2);
  double z = std::sqrt(1 - r2);

  return Vector3{x, y, z};
}
