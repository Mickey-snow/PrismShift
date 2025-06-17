#pragma once

#include "bsdf.hpp"
#include "trowbridge_reitz_distribution.hpp"
#include "util/random.hpp"

namespace bxdfs {

class Dielectric : public BxDF {
 public:
  explicit Dielectric(
      double eta,
      TrowbridgeReitzDistribution mfdist = TrowbridgeReitzDistribution(0, 0));

  Color f(const Vector3& wi, const Vector3& wo) const override;
  std::optional<bxdfSample> Sample_f(const Vector3& wi) const override;
  double pdf(const Vector3& wi, const Vector3& wo) const override;

 private:
  double eta;
  TrowbridgeReitzDistribution mfdist_;
};

}  // namespace bxdfs
