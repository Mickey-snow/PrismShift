#pragma once

#include <bsdf.hpp>

namespace bxdfs {

class Lambertian : public BxDF {
 public:
  explicit Lambertian(const Color& _col)
      : BxDF(BxDFBits::Diffuse | BxDFBits::Reflection), col(_col) {}

  Color f(const Vector3&, const Vector3&) const override { return col; }
  std::optional<bxdfSample> Sample_f(
      const Vector3& in_direction) const override;
  double pdf(const Vector3& i_dir, const Vector3& o_dir) const override;

 private:
  Color col;
};

}  // namespace bxdfs
