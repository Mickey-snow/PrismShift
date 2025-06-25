#include "lambertian.hpp"

#include <util/util.hpp>

namespace bxdfs {
using namespace vec_helpers;

// ---------------------------------------------------------------------------
// Lambertian
// ---------------------------------------------------------------------------

Lambertian::Lambertian(Color _col)
    : BxDF(BxDFBits::Diffuse | BxDFBits::Reflection), col(std::move(_col)) {}

Color Lambertian::f(const Vector3& wi, const Vector3& wo) const {
  // f = rho / pi for wi and wo in the same hemisphere
  return SameHemisphere(wi, wo) ? col * invpi : Color(0, 0, 0);
}

std::optional<bxdfSample> Lambertian::Sample_f(const Vector3& wi) const {
  // Sample wo ~ cos(theta)/pi
  auto wo = Spawn_cosine_distributed_hemisphere();
  wo.y() *= wi.y() > 0 ? -1 : 1;
  return bxdfSample(col * invpi, wo, pdf(wi, wo),
                    BxDFBits::Diffuse | BxDFBits::Reflection);
}

Float Lambertian::pdf(const Vector3& wi, const Vector3& wo) const {
  if (!SameHemisphere(wi, wo))
    return 0;
  // p(wo) = cos(theta_o)/pi
  return std::fabs(wo.y()) * invpi;
}

}  // namespace bxdfs
