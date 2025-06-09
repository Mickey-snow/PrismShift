#include "conductor.hpp"
#include <util/util.hpp>

Vector3 Conductor_BRDF::Reflected_Direction(const Vector3& wi) const {
  return Vector3{wi.x(), wi.y(), -wi.z()};
}

Color Conductor_BRDF::f(const Vector3& wi, const Vector3& wo) const {
  if (pdf(wi, wo) > 0)
    return col;
  else
    return Color{0, 0, 0};
}

double Conductor_BRDF::pdf(const Vector3& wi, const Vector3& wo) const {
  if (isSpecular)
    return 0;

  auto delta_w = wo - Reflected_Direction(wi);
  if (delta_w.Length_squared() <= fuzz * fuzz + 1e-7)
    return pdf_cosine_distributed_hemisphere(delta_w);
  else
    return 0;
}

bxdfSample Conductor_BRDF::Sample_f(const Vector3& wi) const {
  auto wo = Reflected_Direction(wi);
  if (isSpecular)
    return BxDF::Make_Sample(col, wo, 1.0, this);

  auto delta_w = Spawn_cosine_distributed_hemisphere();
  return BxDF::Make_Sample(col, wo + delta_w * fuzz,
                           pdf_cosine_distributed_hemisphere(delta_w), this);
}
