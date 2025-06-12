#include "conductor.hpp"
#include <util/util.hpp>

namespace bxdfs {

inline static Vector3 ReflectedDirection(Vector3 wi) {
  return Vector3(
      basic_vector<double, 3>{-wi.x(), -wi.y(), wi.z()}.normalised());
}

inline static double AbsCosTheta(Vector3 v) {
  // supposing v.Length() == 1
  return std::abs(v.z());
}

Color Conductor::f(const Vector3& wi, const Vector3& wo) const {
  if (pdf(wi, wo) > 0)
    return col;
  else
    return Color{0, 0, 0};
}

double Conductor::pdf(const Vector3& wi, const Vector3& wo) const {
  if (isSpecular)
    return 0;

  auto delta_w = wo - ReflectedDirection(wi);
  if (delta_w.Length_squared() <= fuzz * fuzz + 1e-7)
    return pdf_cosine_distributed_hemisphere(delta_w);
  else
    return 0;
}

std::optional<bxdfSample> Conductor::Sample_f(const Vector3& wi) const {
  auto wo = ReflectedDirection(wi);
  if (isSpecular) {
    return BxDF::MakeSample(col, wo, 1.0, this);
  }

  auto delta_w = Spawn_cosine_distributed_hemisphere();
  return BxDF::MakeSample(col, wo + delta_w * fuzz,
                           pdf_cosine_distributed_hemisphere(delta_w), this);
}

}  // namespace bxdfs
