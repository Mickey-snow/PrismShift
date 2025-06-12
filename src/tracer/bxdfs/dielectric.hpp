#pragma once

#include "bsdf.hpp"
#include "util/random.hpp"

namespace bxdfs {

class Dielectric : public BxDF {
 public:
  explicit Dielectric(double index)
      : BxDF(BxDFBits::Reflection | BxDFBits::Transmission |
             BxDFBits::Specular),
        eta(index) {}

  Color f(const Vector3& wi, const Vector3& wo) const override;
  std::optional<bxdfSample> Sample_f(const Vector3& wi) const override;
  double pdf(const Vector3& wi, const Vector3& wo) const override;

 private:
  double eta;
};

}  // namespace bxdfs
