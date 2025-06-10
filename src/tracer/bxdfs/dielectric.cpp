#include "dielectric.hpp"

#include <cmath>

namespace bxdfs {

Vector3 Dielectric::Reflect(const Vector3& wi) { return {wi.x(), wi.y(), -wi.z()}; }

std::optional<Vector3> Dielectric::Refract(const Vector3& wi, double etaRel) {
  double cosThetaI = wi.z();
  double sin2ThetaI = std::max(0.0, 1.0 - cosThetaI * cosThetaI);
  double sin2ThetaT = etaRel * etaRel * sin2ThetaI;
  if (sin2ThetaT > 1.0) return std::nullopt;  // total internal reflection
  double cosThetaT = std::sqrt(1.0 - sin2ThetaT);
  return Vector3{-etaRel * wi.x(), -etaRel * wi.y(), cosThetaI > 0 ? -cosThetaT : cosThetaT};
}

double Dielectric::FresnelSchlick(double cosTheta, double etaI, double etaT) {
  double r0 = (etaI - etaT) / (etaI + etaT);
  r0 *= r0;
  double t = 1.0 - cosTheta;
  return r0 + (1.0 - r0) * t * t * t * t * t;
}

Color Dielectric::f(const Vector3&, const Vector3&) const { return {0, 0, 0}; }

double Dielectric::pdf(const Vector3&, const Vector3&) const { return 0.0; }

std::optional<bxdfSample> Dielectric::Sample_f(const Vector3& wi) const {
  bool entering = wi.z() > 0;
  double etaI = entering ? 1.0 : eta;
  double etaT = entering ? eta : 1.0;
  double etaRel = etaI / etaT;

  Vector3 wr = Reflect(wi);
  auto wtOpt = Refract(wi, etaRel);

  double cosThetaI = std::fabs(wi.z());
  double reflectProb = wtOpt ? FresnelSchlick(cosThetaI, etaI, etaT) : 1.0;

  if (!wtOpt || random_uniform_01() < reflectProb)
    return BxDF::Make_Sample(Color{1, 1, 1}, wr, 1.0, this);

  return BxDF::Make_Sample(Color{1, 1, 1}, *wtOpt, 1.0, this);
}

}  // namespace bxdfs

