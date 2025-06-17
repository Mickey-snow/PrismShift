#include "conductor.hpp"

#include "util/util.hpp"

#include <spdlog/spdlog.h>

namespace bxdfs {

Conductor::Conductor(TrowbridgeReitzDistribution mfdist, Color col)
    : BxDF(BxDFBits::Reflection),
      mfdist_(std::move(mfdist)),
      col_(std::move(col)) {
  if (mfdist_.EffectivelySmooth())
    SetFlags(GetFlags() | BxDFBits::Specular);
  else
    SetFlags(GetFlags() | BxDFBits::Glossy);
}

Color Conductor::f(const Vector3& wi, const Vector3& wo) const {
  if (mfdist_.EffectivelySmooth() || !SameHemisphere(wi, wo))
    return Color(0);

  Vector3 wm = wo - wi;
  double cos0_i = AbsCosTheta(wi), cos0_o = AbsCosTheta(wo);
  if (cos0_i == 0 || cos0_o == 0 || wm.NearZero())
    return Color(0);

  wm = wm.Normalized();
  double throughput = mfdist_.D(wm) * mfdist_.G(wi, wo) / (4 * cos0_i * cos0_o);
  return throughput * col_;
}

double Conductor::pdf(const Vector3& wi, const Vector3& wo) const {
  if (mfdist_.EffectivelySmooth() || !SameHemisphere(wi, wo))
    return 0;

  Vector3 wm = wo - wi;
  if (wm.NearZero())
    return 0;

  // face upward
  wm = (wm.y() < 0 ? -1 : 1) * wm.Normalized();
  return mfdist_.PDF(wi, wm) / (4 * std::abs(wo.Dot(wm)));
}

std::optional<bxdfSample> Conductor::Sample_f(const Vector3& wi) const {
  if (mfdist_.EffectivelySmooth()) {
    Vector3 wo(wi.x(), -wi.y(), wi.z());
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
