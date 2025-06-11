#include "bsdf.hpp"

Color BSDF::f(const Vector3& rin, const Vector3& rout, BxDFBits flag) const {
  if (bxdf != nullptr && bxdf->MatchesFlag(flag)) {
    return bxdf->f(rin, rout);
  } else
    return Color{0, 0, 0};
}

std::optional<bxdfSample> BSDF::Sample_f(const Vector3& rin) const {
  if (bxdf != nullptr) {
    auto sample_opt = bxdf->Sample_f(rin);
    if (!sample_opt)
      return std::nullopt;
    auto sample = *sample_opt;
    return sample;
  }
  return std::nullopt;
}

double BSDF::pdf(const Vector3& rin, const Vector3& rout, BxDFBits flag) const {
  if (bxdf != nullptr && bxdf->MatchesFlag(flag)) {
    return bxdf->pdf(rin, rout);
  }
  return 0.0;
}
