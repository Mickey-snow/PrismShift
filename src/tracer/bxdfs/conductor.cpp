#include "conductor.hpp"

#include "util/util.hpp"

#include <spdlog/spdlog.h>

namespace bxdfs {

namespace {
inline static Vector3 Reflect(const Vector3& v) {
  return {v.x(), -v.y(), v.z()};
}
}  // namespace

inline static double AbsCosTheta(const Vector3& v) { return std::abs(v.y()); }

Conductor::Conductor(Color _col, double f)
    : BxDF(BxDFBits::Reflection),
      isSpecular(fabs(f) < 1e-6),
      fuzz(fabs(f) > 1 ? 1 : fabs(f)),
      col(std::move(_col)) {
  if (!isSpecular)
    spdlog::error("rough conductor BRDF not supported yet. (f = {})", f);

  if (isSpecular)
    SetFlags(GetFlags() | BxDFBits::Specular);
  else
    SetFlags(GetFlags() | BxDFBits::Glossy);
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

  auto delta_w = wo - Reflect(wi);
  if (delta_w.Length_squared() <= fuzz * fuzz + 1e-7)
    return pdf_cosine_distributed_hemisphere(delta_w);
  else
    return 0;
}

std::optional<bxdfSample> Conductor::Sample_f(const Vector3& wi) const {
  Vector3 wo = Reflect(wi);
  // if (isSpecular)
  return bxdfSample(col, wo, 1.0, BxDFBits::Reflection | BxDFBits::Specular);
}

}  // namespace bxdfs
