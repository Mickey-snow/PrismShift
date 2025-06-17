#pragma once

#include "bsdf.hpp"
#include "trowbridge_reitz_distribution.hpp"
#include "util/vector_fwd.hpp"

namespace bxdfs {

class Conductor : public BxDF {
 public:
  explicit Conductor(TrowbridgeReitzDistribution mfdist, Color col);

  Color f(const Vector3& wi, const Vector3& wo) const override;
  std::optional<bxdfSample> Sample_f(const Vector3& wi) const override;
  double pdf(const Vector3& wi, const Vector3& wo) const override;

 private:
  TrowbridgeReitzDistribution mfdist_;
  double fuzz_;
  Color col_;
};

}  // namespace bxdfs
