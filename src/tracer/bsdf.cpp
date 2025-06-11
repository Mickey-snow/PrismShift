#include "bsdf.hpp"

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

  Color kd = rec.material->diffuse->Evaluate(uv);
  bxdf_ = std::make_shared<bxdfs::Lambertian>(kd);
}

Color BSDF::f(const Vector3& rin, const Vector3& rout, BxDFBits flag) const {
  if (bxdf_ != nullptr && bxdf_->MatchesFlag(flag)) {
    return bxdf_->f(rin.Transform(transform), rout.Transform(transform));
  } else
    return Color{0, 0, 0};
}

std::optional<bxdfSample> BSDF::Sample_f(const Vector3& rin) const {
  if (bxdf_ != nullptr) {
    auto sample_opt = bxdf_->Sample_f(rin.Transform(transform));
    if (!sample_opt)
      return std::nullopt;
    auto sample = *sample_opt;
    sample.wo = transform.Undo(sample.wo);
    return sample;
  }
  return std::nullopt;
}

double BSDF::pdf(const Vector3& rin, const Vector3& rout, BxDFBits flag) const {
  if (bxdf_ != nullptr && bxdf_->MatchesFlag(flag)) {
    return bxdf_->pdf(rin, rout);
  }
  return 0.0;
}
