#include "bsdf.hpp"

Color BSDF::f(const Vector3& rin,
              const Vector3& rout,
              const BxDFType& flag) const {
  if (bxdf != nullptr && bxdf->MatchesFlag(flag)) {
    auto rin_local = frame.World2Local(rin),
         rout_local = frame.World2Local(rout);
    return bxdf->f(rin_local, rout_local);
  } else
    return Color{0, 0, 0};
}

bxdfSample BSDF::Sample_f(const Vector3& rin) const {
  if (bxdf != nullptr) {
    auto sample = bxdf->Sample_f(frame.World2Local(rin));
    sample.out_direction = frame.Local2World(sample.out_direction);
    return sample;
  } else
    return {};
}

double BSDF::pdf(const Vector3& rin,
                 const Vector3& rout,
                 const BxDFType& flag) const {
  double pdf = 0;
  if (bxdf != nullptr && bxdf->MatchesFlag(flag)) {
    auto rin_local = frame.World2Local(rin),
         rout_local = frame.World2Local(rout);
    pdf = bxdf->pdf(rin_local, rout_local);
  }
  return pdf;
}
