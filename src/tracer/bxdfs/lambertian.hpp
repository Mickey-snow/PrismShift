#pragma once

#include <bsdf.hpp>

class Lambertian_BRDF : public BxDF {
 public:
  Lambertian_BRDF(const Color& _col)
      : col(_col), BxDF(BxDFType::Diffuse | BxDFType::Reflection) {}

  Color f(const Vector3&, const Vector3&) const override { return col; }
  bxdfSample Sample_f(const Vector3& in_direction) const override;
  double pdf(const Vector3& i_dir, const Vector3& o_dir) const override;

 private:
  Color col;
};
