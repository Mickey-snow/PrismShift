#include "dielectric.hpp"

#include <cmath>

namespace bxdfs {
using namespace vec_helpers;

namespace {
// helpers
static double FrDielectric(double cos0, double eta) {
  cos0 = std::clamp<double>(cos0, -1, 1);
  // Potentially flip interface orientation for Fresnel equations
  if (cos0 < 0) {
    eta = 1 / eta;
    cos0 = -cos0;
  }

  // Compute $\cos\,\theta_\roman{t}$ for Fresnel equations using Snell's law
  double sin2Theta_o = 1 - Sqr(cos0);
  double sin2Theta_t = sin2Theta_o / Sqr(eta);
  if (sin2Theta_t >= 1)
    return 1.f;
  double cosTheta_t = std::sqrt(1 - sin2Theta_t);

  double r_parl = (eta * cos0 - cosTheta_t) / (eta * cos0 + cosTheta_t);
  double r_perp = (cos0 - eta * cosTheta_t) / (cos0 + eta * cosTheta_t);
  return (Sqr(r_parl) + Sqr(r_perp)) / 2;
}

}  // namespace

Dielectric::Dielectric(double index, TrowbridgeReitzDistribution mfdist)
    : BxDF(BxDFBits::Transmission), eta(index), mfdist_(std::move(mfdist)) {
  if (eta != 1)
    SetFlags(GetFlags() | BxDFBits::Reflection);
  SetFlags(GetFlags() | (mfdist_.EffectivelySmooth() ? BxDFBits::Specular
                                                     : BxDFBits::Glossy));
}

Color Dielectric::f(const Vector3& wi, const Vector3& wo) const {
  if (mfdist_.EffectivelySmooth() || eta == 1)
    return Color(0);

  double cos0_i = CosTheta(-wi), cos0_o = CosTheta(wo);
  bool is_reflect = cos0_i * cos0_o > 0;

  double etap = 1;
  if (!is_reflect)
    etap = cos0_i > 0 ? eta : (1.0 / eta);

  Vector3 wm = wo * etap - wi;
  if (cos0_i == 0 || cos0_o == 0 || wm.NearZero())
    return Color(0);

  wm = (wm.y() < 0 ? -1 : 1) * wm.Normalized();

  if (wm.Dot(wo) * cos0_o < 0 || wm.Dot(-wi) * cos0_i < 0)
    return Color(0);

  double F = FrDielectric(wm.Dot(-wi), eta);
  if (is_reflect) {
    double c =
        mfdist_.D(wm) * mfdist_.G(wi, wo) * F / std::abs(4 * cos0_i * cos0_o);
    return Color(c);
  } else {
    double denom = Sqr(wm.Dot(wo) + wm.Dot(-wi) / etap) * cos0_i * cos0_o;
    double ft = mfdist_.D(wm) * (1 - F) * mfdist_.G(wi, wo) *
                std::abs(wm.Dot(wo) * wm.Dot(-wi) / denom);

    ft /= Sqr(etap);
    return Color(ft);
  }
}

double Dielectric::pdf(const Vector3& wi, const Vector3& wo) const {
  if (mfdist_.EffectivelySmooth() || eta == 1)
    return 0;

  double cos0_i = CosTheta(-wi), cos0_o = CosTheta(wo);
  bool is_reflect = cos0_i * cos0_o > 0;

  double etap = 1;
  if (!is_reflect)
    etap = cos0_i > 0 ? eta : (1.0 / eta);

  Vector3 wm = wo * etap - wi;
  if (cos0_i == 0 || cos0_o == 0 || wm.NearZero())
    return 0;

  wm = (wm.y() < 0 ? -1 : 1) * wm.Normalized();

  if (wm.Dot(wo) * cos0_o < 0 || wm.Dot(-wi) * cos0_i < 0)
    return 0;

  double pr = FrDielectric(wm.Dot(-wi), eta);
  double pt = 1.0 - pr;

  double pdf = 0;
  if (is_reflect) {
    pdf = pr * mfdist_.PDF(-wi, wm) / (4 * std::abs(wm.Dot(wi)));
  } else {
    double denom = Sqr(wm.Dot(wo) + wm.Dot(-wi) / etap);
    double dwm_dwo = std::abs(wm.Dot(wo)) / denom;
    pdf = mfdist_.PDF(-wi, wm) * dwm_dwo * pt;
  }
  return pdf;
}

std::optional<bxdfSample> Dielectric::Sample_f(const Vector3& wi) const {
  bool entering = wi.y() < 0;
  double etaI = entering ? 1.0 : eta;
  double etaT = entering ? eta : 1.0;
  double etaRel = etaI / etaT;

  if (eta == 1 || mfdist_.EffectivelySmooth()) {
    double pr = FrDielectric(CosTheta(-wi), eta);
    double pt = 1 - pr;

    if (random_uniform_01() < pr) {
      Vector3 wo(wi.x(), -wi.y(), wi.z());
      Color fr(pr / AbsCosTheta(wo));
      return bxdfSample(fr, wo, pr, BxDFBits::Specular | BxDFBits::Reflection);
    } else {
      Vector3 wo;

      bool ok = Refract(wi, Vector3(0, 1, 0), etaRel, &wo);
      if (!ok)
        return std::nullopt;

      Color ft(pt / AbsCosTheta(wo));
      return bxdfSample(ft, wo, pt,
                        BxDFBits::Specular | BxDFBits::Transmission);
    }
  } else {
    Vector3 wm = mfdist_.Sample_wm(-wi);
    double pr = FrDielectric(wm.Dot(-wi), eta);
    double pt = 1.0 - pr;

    if (random_uniform_01() < pr) {
      Vector3 wo = Reflect(wi, wm).Normalized();
      if (!SameHemisphere(wi, wo))
        return std::nullopt;

      double pdf = mfdist_.PDF(-wi, wm) / (4 * std::abs(wm.Dot(wi))) * pr;
      double f = mfdist_.D(wm) * mfdist_.G(wi, wo) * pr /
                 (4 * CosTheta(wo) * CosTheta(-wi));
      return bxdfSample(Color(f), wo, pdf,
                        BxDFBits::Glossy | BxDFBits::Reflection);
    } else {
      Vector3 wo;
      double etap = etaRel;
      bool ok = Refract(wi, wm, etap, &wo);
      if (!ok || SameHemisphere(wi, wo) || wo.y() == 0)
        return std::nullopt;

      wo = wo.Normalized();
      double denom = Sqr(wm.Dot(wo) + wm.Dot(-wi) / etap);
      double dwm_dwo = std::abs(wm.Dot(wo)) / denom;
      double pdf = mfdist_.PDF(-wi, wm) * dwm_dwo * pt;
      double ft = pt * mfdist_.D(wm) * mfdist_.G(wi, wo) / denom *
                  std::abs(wm.Dot(wo) * wm.Dot(-wi) /
                           (AbsCosTheta(wo) * AbsCosTheta(wi)));

      if (pdf == 0)
        return std::nullopt;

      return bxdfSample(Color(ft), wo, pdf,
                        BxDFBits::Glossy | BxDFBits::Transmission);
    }
  }
}

}  // namespace bxdfs
