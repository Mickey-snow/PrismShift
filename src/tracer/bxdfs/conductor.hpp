#pragma once

#include "bsdf.hpp"
#include "util/vector_fwd.hpp"

namespace bxdfs {

class Conductor : public BxDF {
 public:
  Conductor(Color _col, double f = 0)
      : BxDF(BxDFBits::Reflection),
        isSpecular(fabs(f) < 1e-6),
        fuzz(fabs(f) > 1 ? 1 : fabs(f)),
        col(std::move(_col)) {
    if (isSpecular)
      SetFlags(GetFlags() | BxDFBits::Specular);
    else
      SetFlags(GetFlags() | BxDFBits::Glossy);
  }

  Color f(const Vector3&, const Vector3&) const override;
  std::optional<bxdfSample> Sample_f(const Vector3&) const override;
  double pdf(const Vector3&, const Vector3&) const override;

 private:
  bool isSpecular;
  double fuzz;
  Color col;
};

}  // namespace bxdfs
