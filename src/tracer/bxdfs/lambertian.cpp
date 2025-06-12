#include "lambertian.hpp"

#include <util/util.hpp>

namespace bxdfs {

namespace {  // helper
inline static bool SameHemisphere(const Vector3& w, const Vector3& wp) {
  return w.y() * wp.y() >= 0.0;
}
}  // namespace

Lambertian::Lambertian(Color _col)
    : BxDF(BxDFBits::Diffuse | BxDFBits::Reflection), col(std::move(_col)) {}

Color Lambertian::f(const Vector3& wi, const Vector3& wo) const {
  return SameHemisphere(wi, wo) ? col * invpi : Color(0, 0, 0);
}

std::optional<bxdfSample> Lambertian::Sample_f(const Vector3& wi) const {
  auto wo = Spawn_cosine_distributed_hemisphere();
  wo.y() *= wi.y() < 0 ? -1 : 1;
  return BxDF::MakeSample(col * invpi, wo, pdf(wi, wo), this);
}

double Lambertian::pdf(const Vector3& wi, const Vector3& wo) const {
  if (!SameHemisphere(wi, wo))
    return 0;
  // return pdf_cosine_distributed_hemisphere(wo);
  return std::fabs(wo.y()) * invpi;
}

}  // namespace bxdfs
