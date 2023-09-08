#include "dielectric.hpp"
#include<util/util.hpp>
#include<renderer.hpp>

#include<cmath>


double Reflectance(const double& cosine, const double& ref_idx){
  // Use Schlick's approximation for reflectance.
  auto r0 = (1-ref_idx) / (1+ref_idx);
  r0 = r0*r0;
  return r0+(1-r0)*pow((1-cosine), 5);
}


Color Dielectric::Ray_Color(const Hit_record& rec) const {
  double eta_ratio = rec.front_face ? (1.0/eta) : eta;

  double cos_theta = fmin(Vector3::Dot(-rec.ray.Direction().Unit(), rec.normal), 1.0);
  double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

  if(eta_ratio*sin_theta > 1.0 or Reflectance(cos_theta, eta_ratio) > random_uniform_01()){
    // Total reflection
    Vector3 reflected_direction = rec.ray.Reflect_Direction(rec.normal);
    return Renderer::Instance()->Ray_Color(Ray(rec.position, reflected_direction), rec.hit_counts);
  }
  else{
    // Refract
    Vector3 refracted_direction = rec.ray.Refract_Direction(rec.normal, eta_ratio);
    return Renderer::Instance()->Ray_Color(Ray(rec.position, refracted_direction), rec.hit_counts);
  }      
}
