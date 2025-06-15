#include "bsdf.hpp"

#include "bxdfs/conductor.hpp"
#include "bxdfs/dielectric.hpp"
#include "bxdfs/lambertian.hpp"
#include "hit_record.hpp"
#include "material.hpp"
#include "texture.hpp"

BSDF BSDF::Create(Normal normal, std::shared_ptr<Material> mat) {
  std::shared_ptr<BxDF> bxdf_ = nullptr;

  if (mat) {
    // TODO: texture coordinate and mapping
    Point2 uv(0, 0);

    if (mat->diffuse && !mat->dielectric) {
      Color kd = mat->diffuse->Evaluate(uv);
      bxdf_ = std::make_shared<bxdfs::Lambertian>(kd);
    } else if (mat->diffuse && mat->dielectric) {
      Color c = mat->diffuse->Evaluate(uv);
      double d = mat->dielectric->Evaluate(uv);
      bxdf_ = std::make_shared<bxdfs::Conductor>(std::move(c), d);
    } else if (!mat->diffuse && mat->dielectric) {
      double d = mat->dielectric->Evaluate(uv);
      bxdf_ = std::make_shared<bxdfs::Dielectric>(d);
    }
  }

  return BSDF(bxdf_, QuaternionTransform::RotateFrTo(Vector3(normal),
                                                     Vector3{0, 1, 0}));
}

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

double BSDF::pdf(Vector3 wi, Vector3 wo, BxDFBits flag) const {
  wi = transform.Doit(wi).Normalized();
  wo = transform.Doit(wo).Normalized();
  if (bxdf_ != nullptr && bxdf_->MatchesFlag(flag))
    return bxdf_->pdf(wi, wo);
  return 0.0;
}
