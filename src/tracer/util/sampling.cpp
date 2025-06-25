#include "sampling.hpp"
#include <util/constant.hpp>
#include <util/random.hpp>
#include <util/vector.hpp>

Float pdf_cosine_distributed_hemisphere(const Vector3& wo) {
  const Float len_sq = wo.Length_squared();
  if (len_sq == 0)
    return invpi;  // treat as aligned with +Z
  if (wo.y() <= 0)
    return 0;
  Float costheta = wo.y() / std::sqrt(len_sq);
  return costheta * invpi;
}

Vector3 Spawn_cosine_distributed_hemisphere() {
  Float r1 = random_uniform_01(), r2 = random_uniform_01();
  Float phi = r1 * 2 * pi;

  Float dx = cos(phi) * std::sqrt(r2);
  Float dz = sin(phi) * std::sqrt(r2);
  Float dy = std::sqrt(1 - r2);

  return Vector3{dx, dy, dz};
}
