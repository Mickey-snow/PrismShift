#include "conductor.hpp"

#include "util/util.hpp"

#include <spdlog/spdlog.h>

namespace bxdfs {

namespace {
inline static Vector3 Reflect(const Vector3& v) {
  return {v.x(), -v.y(), v.z()};
}
}  // namespace

Conductor::Conductor(TrowbridgeReitzDistribution mfdist, Color color, double f)
    : BxDF(BxDFBits::Reflection),
      mfdist_(std::move(mfdist)),
      fuzz_(fabs(f) > 1 ? 1 : fabs(f)),
      col_(std::move(color)) {
  if (mfdist_.EffectivelySmooth())
    SetFlags(GetFlags() | BxDFBits::Specular);
  else
    SetFlags(GetFlags() | BxDFBits::Glossy);
}

Color Conductor::f(const Vector3& wi, const Vector3& wo) const {
  if (pdf(wi, wo) > 0)
    return col_;

  return Color{0, 0, 0};
}

double Conductor::pdf(const Vector3& wi, const Vector3& wo) const {
  if (mfdist_.EffectivelySmooth())
    return 0;

  return 0;
}

std::optional<bxdfSample> Conductor::Sample_f(const Vector3& wi) const {
  if (mfdist_.EffectivelySmooth()) {
    Vector3 wo = Reflect(wi);
    return bxdfSample(col_, wo, 1.0, BxDFBits::Reflection | BxDFBits::Specular);
  }

  if (wi.y() == 0)
    return std::nullopt;

  Vector3 wm = mfdist_.Sample_wm(-wi);
  Vector3 wo = Reflect(wi, wm);  // ?
  if (!SameHemisphere(wi, wo))
    return std::nullopt;

  double costheta_i = AbsCosTheta(wi), costheta_o = AbsCosTheta(wo);
  if (costheta_i == 0 || costheta_o == 0)
    return std::nullopt;

  double pdf = mfdist_.PDF(wi, wm) / (4.0 * std::fabs(wi.Dot(wm)));
  Color f = mfdist_.D(wm) * col_ * mfdist_.G(wi, wo) /
            (4.0 * costheta_i * costheta_o);
  return bxdfSample(f, wo, pdf, BxDFBits::Glossy | BxDFBits::Reflection);
}

}  // namespace bxdfs
