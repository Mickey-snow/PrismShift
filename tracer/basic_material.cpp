#include "material.hpp"
#include "basic_material.hpp"
#include "renderer.hpp"
#include "util/util.hpp"
#include "factory.hpp"

#include<cmath>

Color Lambertian::Ray_Color(const Ray& r,const Hit_record& rec) const {
  Vector3 scattered_direction = Ray::Scatter_Direction(rec.front_face ? rec.normal : -rec.normal);
  return attenuation * Renderer::Instance()->Ray_Color(Ray(rec.position, scattered_direction), rec.hit_counts);
}


Color Metal::Ray_Color(const Ray& r,const Hit_record& rec) const {
  Vector3 reflected_direction = Ray::Reflect_Direction(r.Direction(), rec.front_face ? rec.normal : rec.normal);
  reflected_direction = reflected_direction + fuzz*Vector3::Random_Unit();
  return attenuation * Renderer::Instance()->Ray_Color(Ray(rec.position, reflected_direction), rec.hit_counts);
}


double Reflectance(const double& cosine, const double& ref_idx){
  // Use Schlick's approximation for reflectance.
  auto r0 = (1-ref_idx) / (1+ref_idx);
  r0 = r0*r0;
  return r0+(1-r0)*pow((1-cosine), 5);
}
Color Dielectric::Ray_Color(const Ray& r, const Hit_record& rec) const {
  double eta_ratio = rec.front_face ? (1.0/eta) : eta;

  double cos_theta = fmin(Dot(-r.Direction().Unit(), rec.normal), 1.0);
  double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

  if(eta_ratio*sin_theta > 1.0 or Reflectance(cos_theta, eta_ratio) > random_uniform_01()){
    // Total reflection
    Vector3 reflected_direction = Ray::Reflect_Direction(r.Direction(), rec.front_face ? rec.normal : rec.normal);
    return Renderer::Instance()->Ray_Color(Ray(rec.position, reflected_direction), rec.hit_counts);
  }
  else{
    // Refract
    Vector3 refracted_direction = Ray::Refract_Direction(r.Direction().Unit(), rec.normal, eta_ratio);
    return Renderer::Instance()->Ray_Color(Ray(rec.position, refracted_direction), rec.hit_counts);
  }      
}
