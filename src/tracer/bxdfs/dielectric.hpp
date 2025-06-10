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

  Color f(const Vector3&, const Vector3&) const override;
  std::optional<bxdfSample> Sample_f(const Vector3& in) const override;
  double pdf(const Vector3&, const Vector3&) const override;

 private:
  double eta;

  static Vector3 Reflect(const Vector3& v);
  static std::optional<Vector3> Refract(const Vector3& v, double etaRel);
  static double FresnelSchlick(double cosTheta, double etaI, double etaT);
};

}  // namespace bxdfs
