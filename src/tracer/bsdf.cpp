#include "bsdf.hpp"

Color BSDF::f(const Vector3& rin,
              const Vector3& rout,
              BxDFBits flag) const {
  if (bxdf != nullptr && bxdf->MatchesFlag(flag)) {
    auto rin_local = frame.World2Local(rin),
         rout_local = frame.World2Local(rout);
    return bxdf->f(rin_local, rout_local);
  } else
    return Color{0, 0, 0};
}

std::optional<bxdfSample> BSDF::Sample_f(const Vector3& rin) const {
  if (bxdf != nullptr) {
    auto sample_opt = bxdf->Sample_f(frame.World2Local(rin));
    if (!sample_opt) return std::nullopt;
    auto sample = *sample_opt;
    sample.out_direction = frame.Local2World(sample.out_direction);
    return sample;
  }
  return std::nullopt;
}

double BSDF::pdf(const Vector3& rin,
                 const Vector3& rout,
                 BxDFBits flag) const {
  double pdf = 0;
  if (bxdf != nullptr && bxdf->MatchesFlag(flag)) {
    auto rin_local = frame.World2Local(rin),
         rout_local = frame.World2Local(rout);
    pdf = bxdf->pdf(rin_local, rout_local);
  }
  return pdf;
}
