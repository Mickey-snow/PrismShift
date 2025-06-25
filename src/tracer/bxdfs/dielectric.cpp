#include "dielectric.hpp"

#include <cmath>

namespace bxdfs {
using namespace vec_helpers;

namespace {
// Fresnel reflectance for dielectrics using unpolarized light
static Float FrDielectric(Float cos0, Float eta) {
  cos0 = std::clamp<Float>(cos0, -1, 1);
  // Potentially flip interface orientation for Fresnel equations
  if (cos0 < 0) {
    eta = 1 / eta;
    cos0 = -cos0;
  }

  // Compute \f$\cos\theta_t\f$ for Fresnel equations using Snell's law
  Float sin2Theta_o = 1 - Sqr(cos0);
  Float sin2Theta_t = sin2Theta_o / Sqr(eta);
  if (sin2Theta_t >= 1)
    return 1.f;
  Float cosTheta_t = std::sqrt(1 - sin2Theta_t);

  Float r_parl = (eta * cos0 - cosTheta_t) / (eta * cos0 + cosTheta_t);
  Float r_perp = (cos0 - eta * cosTheta_t) / (cos0 + eta * cosTheta_t);
  return (Sqr(r_parl) + Sqr(r_perp)) / 2;
}

}  // namespace

Dielectric::Dielectric(Float index, TrowbridgeReitzDistribution mfdist)
    : BxDF(BxDFBits::Transmission), eta(index), mfdist_(std::move(mfdist)) {
  if (eta != 1)
    SetFlags(GetFlags() | BxDFBits::Reflection);
  SetFlags(GetFlags() | (mfdist_.EffectivelySmooth() ? BxDFBits::Specular
                                                     : BxDFBits::Glossy));
}

Color Dielectric::f(const Vector3& wi, const Vector3& wo) const {
  // Rough dielectric BRDF/BTDF evaluation
  if (mfdist_.EffectivelySmooth() || eta == 1)
    return Color(0);

  // Cosine of incident and outgoing angles w.r.t. the surface normal
  Float cos0_i = CosTheta(-wi), cos0_o = CosTheta(wo);
  bool is_reflect = cos0_i * cos0_o > 0;

  Float etap = 1;
  if (!is_reflect)
    etap = cos0_i > 0 ? eta : (1.0 / eta);

  // Microfacet half vector for the interaction
  Vector3 wm = wo * etap - wi;
  if (cos0_i == 0 || cos0_o == 0 || wm.NearZero())
    return Color(0);

  wm = (wm.y() < 0 ? -1 : 1) * wm.Normalized();

  if (wm.Dot(wo) * cos0_o < 0 || wm.Dot(-wi) * cos0_i < 0)
    return Color(0);

  Float F = FrDielectric(wm.Dot(-wi), eta);  // Fresnel term
  if (is_reflect) {
    // Reflection term of the microfacet model
    Float c =
        mfdist_.D(wm) * mfdist_.G(wi, wo) * F / std::abs(4 * cos0_i * cos0_o);
    return Color(c);
  } else {
    Float denom = Sqr(wm.Dot(wo) + wm.Dot(-wi) / etap) * cos0_i * cos0_o;
    Float ft = mfdist_.D(wm) * (1 - F) * mfdist_.G(wi, wo) *
               std::abs(wm.Dot(wo) * wm.Dot(-wi) / denom);

    ft /= Sqr(etap);
    return Color(ft);
  }
}

Float Dielectric::pdf(const Vector3& wi, const Vector3& wo) const {
  // PDF for the rough dielectric model
  if (mfdist_.EffectivelySmooth() || eta == 1)
    return 0;

  Float cos0_i = CosTheta(-wi), cos0_o = CosTheta(wo);
  bool is_reflect = cos0_i * cos0_o > 0;

  Float etap = 1;
  if (!is_reflect)
    etap = cos0_i > 0 ? eta : (1.0 / eta);

  // Half vector computed from Snell's law
  Vector3 wm = wo * etap - wi;
  if (cos0_i == 0 || cos0_o == 0 || wm.NearZero())
    return 0;

  wm = (wm.y() < 0 ? -1 : 1) * wm.Normalized();

  if (wm.Dot(wo) * cos0_o < 0 || wm.Dot(-wi) * cos0_i < 0)
    return 0;

  Float pr = FrDielectric(wm.Dot(-wi), eta);
  Float pt = 1.0 - pr;

  Float pdf = 0;
  if (is_reflect) {
    pdf = pr * mfdist_.PDF(-wi, wm) / (4 * std::abs(wm.Dot(wi)));
  } else {
    Float denom = Sqr(wm.Dot(wo) + wm.Dot(-wi) / etap);
    Float dwm_dwo = std::abs(wm.Dot(wo)) / denom;
    pdf = mfdist_.PDF(-wi, wm) * dwm_dwo * pt;
  }
  return pdf;
}

std::optional<bxdfSample> Dielectric::Sample_f(const Vector3& wi) const {
  // Determine if the ray is entering or exiting
  bool entering = wi.y() < 0;
  Float etaI = entering ? 1.0 : eta;
  Float etaT = entering ? eta : 1.0;
  Float etaRel = etaI / etaT;

  if (eta == 1 || mfdist_.EffectivelySmooth()) {
    Float pr = FrDielectric(CosTheta(-wi), eta);
    Float pt = 1 - pr;

    if (random_uniform_01() < pr) {
      // Specular reflection
      Vector3 wo(wi.x(), -wi.y(), wi.z());
      Color fr(pr / AbsCosTheta(wo));
      return bxdfSample(fr, wo, pr, BxDFBits::Specular | BxDFBits::Reflection);
    } else {
      Vector3 wo;

      bool ok = Refract(wi, Vector3(0, 1, 0), etaRel, &wo);
      if (!ok)
        return std::nullopt;

      // Perfect transmission case
      Color ft(pt / AbsCosTheta(wo));
      return bxdfSample(ft, wo, pt,
                        BxDFBits::Specular | BxDFBits::Transmission);
    }
  } else {
    // Sample a microfacet normal for rough surfaces
    Vector3 wm = mfdist_.Sample_wm(-wi);
    Float pr = FrDielectric(wm.Dot(-wi), eta);
    Float pt = 1.0 - pr;

    if (random_uniform_01() < pr) {
      // Reflect about the sampled microfacet normal
      Vector3 wo = Reflect(wi, wm).Normalized();
      if (!SameHemisphere(wi, wo))
        return std::nullopt;

      Float pdf = mfdist_.PDF(-wi, wm) / (4 * std::abs(wm.Dot(wi))) * pr;
      Float f = mfdist_.D(wm) * mfdist_.G(wi, wo) * pr /
                (4 * CosTheta(wo) * CosTheta(-wi));
      return bxdfSample(Color(f), wo, pdf,
                        BxDFBits::Glossy | BxDFBits::Reflection);
    } else {
      Vector3 wo;
      Float etap = etaRel;
      bool ok = Refract(wi, wm, etap, &wo);  // Sample refraction direction
      if (!ok || SameHemisphere(wi, wo) || wo.y() == 0)
        return std::nullopt;

      wo = wo.Normalized();
      Float denom = Sqr(wm.Dot(wo) + wm.Dot(-wi) / etap);
      Float dwm_dwo = std::abs(wm.Dot(wo)) / denom;
      Float pdf = mfdist_.PDF(-wi, wm) * dwm_dwo * pt;
      Float ft = pt * mfdist_.D(wm) * mfdist_.G(wi, wo) / denom *
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
