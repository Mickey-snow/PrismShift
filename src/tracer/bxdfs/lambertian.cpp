#include "lambertian.hpp"

#include <util/util.hpp>

namespace bxdfs {

Lambertian::Lambertian(Color _col)
    : BxDF(BxDFBits::Diffuse | BxDFBits::Reflection), col(std::move(_col)) {}

Color Lambertian::f(const Vector3& wi, const Vector3& wo) const {
  return SameHemisphere(wi, wo) ? col * invpi : Color(0, 0, 0);
}

std::optional<bxdfSample> Lambertian::Sample_f(const Vector3& wi) const {
  auto wo = Spawn_cosine_distributed_hemisphere();
  wo.y() *= wi.y() > 0 ? -1 : 1;
  return bxdfSample(col * invpi, wo, pdf(wi, wo),
                    BxDFBits::Diffuse | BxDFBits::Reflection);
}

double Lambertian::pdf(const Vector3& wi, const Vector3& wo) const {
  if (!SameHemisphere(wi, wo))
    return 0;
  return std::fabs(wo.y()) * invpi;
}

}  // namespace bxdfs
