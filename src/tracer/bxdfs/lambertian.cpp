#include "lambertian.hpp"

#include<util/util.hpp>


bxdfSample Lambertian_BRDF::Sample_f(const Vector3& in_direction) const{
  auto out_direction = Spawn_cosine_distributed_hemisphere();
  return BxDF::Make_Sample(col, out_direction, pdf(in_direction,out_direction), this);
}


double Lambertian_BRDF::pdf(const Vector3& i_dir, const Vector3& o_dir) const{
  return pdf_cosine_distributed_hemisphere(o_dir);
}
