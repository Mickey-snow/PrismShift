#include "bsdf.hpp"

#include "bxdfs/conductor.hpp"
#include "bxdfs/dielectric.hpp"
#include "bxdfs/lambertian.hpp"
#include "hit_record.hpp"
#include "material.hpp"
#include "texture.hpp"

BSDF::BSDF(const HitRecord& rec)
    : bxdf_(nullptr),
      transform(QuaternionTransform::RotateFrTo(Vector3(rec.normal),
                                                Vector3{0, 1, 0})) {
  // TODO: texture coordinate and mapping
  Point2 uv(0, 0);
  Material const* mat = rec.material;

  if (mat->diffuse && !mat->dielectric) {
    Color kd = rec.material->diffuse->Evaluate(uv);
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

Color BSDF::f(Vector3 rin, Vector3 rout, BxDFBits flag) const {
  if (bxdf_ != nullptr && bxdf_->MatchesFlag(flag)) {
    rin = transform.Doit(rin).Normalized();
    rout = transform.Doit(rout).Normalized();
    return bxdf_->f(rin.Transform(transform), rout.Transform(transform));
  }
  return Color{0, 0, 0};
}

std::optional<bxdfSample> BSDF::Sample_f(Vector3 rin) const {
  if (!bxdf_)
    return std::nullopt;

  const auto len = rin.Length();
  rin /= len;

  auto sample_opt = bxdf_->Sample_f(rin.Transform(transform));
  if (!sample_opt)
    return std::nullopt;

  auto sample = *sample_opt;
  sample.wo = transform.Undo(sample.wo);
  sample.wo = len * sample.wo.Normalized();
  return sample;

  return std::nullopt;
}

double BSDF::pdf(Vector3 rin, Vector3 rout, BxDFBits flag) const {
  rin = rin.Normalized();
  rout = rout.Normalized();

  if (bxdf_ != nullptr && bxdf_->MatchesFlag(flag))
    return bxdf_->pdf(rin, rout);
  return 0.0;
}
