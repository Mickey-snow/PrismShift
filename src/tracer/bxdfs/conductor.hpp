#pragma once

#include "bsdf.hpp"
#include "util/vector_fwd.hpp"

namespace bxdfs {

class Conductor : public BxDF {
 public:
  explicit Conductor(Color _col, double f = 0);

  Color f(const Vector3&, const Vector3&) const override;
  std::optional<bxdfSample> Sample_f(const Vector3&) const override;
  double pdf(const Vector3&, const Vector3&) const override;

 private:
  bool isSpecular;
  double fuzz;
  Color col;
};

}  // namespace bxdfs
