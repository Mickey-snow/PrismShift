#include "bsdf.hpp"

#include "bxdfs/conductor.hpp"
#include "bxdfs/dielectric.hpp"
#include "bxdfs/lambertian.hpp"
#include "hit_record.hpp"
#include "material.hpp"
#include "texture.hpp"

BSDF::BSDF(std::shared_ptr<BxDF> bxdf, QuaternionTransform trans)
    : bxdf_(bxdf), transform(std::move(trans)) {}

Color BSDF::f(Vector3 wi, Vector3 wo, BxDFBits flag) const {
  if (bxdf_ != nullptr && bxdf_->MatchesFlag(flag)) {
    wi = transform.Doit(wi).Normalized();
    wo = transform.Doit(wo).Normalized();
    return bxdf_->f(wi, wo);
  }
  return Color{0, 0, 0};
}

std::optional<bxdfSample> BSDF::Sample_f(Vector3 wi) const {
  if (!bxdf_)
    return std::nullopt;

  auto sample_opt = bxdf_->Sample_f(transform.Doit(wi).Normalized());
  if (!sample_opt)
    return std::nullopt;

  auto sample = *sample_opt;
  sample.wo = transform.Undo(sample.wo).Normalized();
  return sample;

  return std::nullopt;
}

Float BSDF::pdf(Vector3 wi, Vector3 wo, BxDFBits flag) const {
  wi = transform.Doit(wi).Normalized();
  wo = transform.Doit(wo).Normalized();
  if (bxdf_ != nullptr && bxdf_->MatchesFlag(flag))
    return bxdf_->pdf(wi, wo);
  return 0.0;
}

bool BSDF::MatchesFlag(BxDFBits flag) const {
  return bxdf_ && bxdf_->MatchesFlag(flag);
}
