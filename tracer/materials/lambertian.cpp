#include "lambertian.hpp"

#include<util/util.hpp>
#include<renderer.hpp>

Color Lambertian::Ray_Color(const Ray& r,const Hit_record& rec) const {
  Vector3 scattered_direction = Ray::Scatter_Direction(rec.front_face ? rec.normal : -rec.normal);
  return attenuation * Renderer::Instance()->Ray_Color(Ray(rec.position, scattered_direction), rec.hit_counts);
}
