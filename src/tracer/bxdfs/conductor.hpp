#pragma once

#include "bsdf.hpp"
#include "trowbridge_reitz_distribution.hpp"
#include "util/vector_fwd.hpp"

namespace bxdfs {

class Conductor : public BxDF {
 public:
  explicit Conductor(TrowbridgeReitzDistribution mfdist,
                     Color color,
                     double f );

  Color f(const Vector3&, const Vector3&) const override;
  std::optional<bxdfSample> Sample_f(const Vector3&) const override;
  double pdf(const Vector3&, const Vector3&) const override;

 private:
  TrowbridgeReitzDistribution mfdist_;
  double fuzz_;
  Color col_;
};

}  // namespace bxdfs
