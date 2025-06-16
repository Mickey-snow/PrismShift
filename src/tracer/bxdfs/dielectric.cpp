#include "dielectric.hpp"

#include <cmath>

namespace bxdfs {

namespace {
// helpers
/* Convenience overload for the tracer’s local shading frame
 * (normal is always +Y = (0, 1, 0)).  Saves a dot-product. */
inline static Vector3 Reflect(const Vector3& v) {
  return {v.x(), -v.y(), v.z()};
}

inline static std::optional<Vector3> Refract(const Vector3& wi, double etaRel) {
  double cosThetaI = wi.y();
  double sin2ThetaI = std::max(0.0, 1.0 - cosThetaI * cosThetaI);
  double sin2ThetaT = etaRel * etaRel * sin2ThetaI;
  if (sin2ThetaT > 1.0)
    return std::nullopt;  // total internal reflection
  double cosThetaT = std::sqrt(1.0 - sin2ThetaT);
  return Vector3{etaRel * wi.x(), cosThetaI > 0 ? cosThetaT : -cosThetaT,
                 etaRel * wi.z()};
}
inline static double FresnelSchlick(double cosTheta, double etaI, double etaT) {
  double r0 = (etaI - etaT) / (etaI + etaT);
  r0 *= r0;
  double t = 1.0 - cosTheta;
  return r0 + (1.0 - r0) * t * t * t * t * t;
}
}  // namespace

Color Dielectric::f(const Vector3&, const Vector3&) const { return {0, 0, 0}; }

double Dielectric::pdf(const Vector3&, const Vector3&) const { return 0.0; }

std::optional<bxdfSample> Dielectric::Sample_f(const Vector3& wi) const {
  bool entering = wi.y() < 0;
  double etaI = entering ? 1.0 : eta;
  double etaT = entering ? eta : 1.0;
  double etaRel = etaI / etaT;

  Vector3 wr = Reflect(wi);
  auto wtOpt = Refract(wi, etaRel);

  double cosThetaI = std::fabs(wi.y());
  double reflectProb = wtOpt ? FresnelSchlick(cosThetaI, etaI, etaT) : 1.0;

  if (!wtOpt || random_uniform_01() < reflectProb)
    return bxdfSample(Color{1, 1, 1}, wr, 1.0,
                      BxDFBits::Specular | BxDFBits::Reflection);

  return bxdfSample(Color{1, 1, 1}, *wtOpt, 1.0,
                    BxDFBits::Specular | BxDFBits::Transmission);
}

}  // namespace bxdfs
