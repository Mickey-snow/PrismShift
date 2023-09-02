#include "metal.hpp"

#include<renderer.hpp>
#include<util/util.hpp>

Color Metal::Ray_Color(const Ray& r,const Hit_record& rec) const {
  Vector3 reflected_direction = Ray::Reflect_Direction(r.Direction(), rec.front_face ? rec.normal : rec.normal);
  reflected_direction = reflected_direction + fuzz*Vector3::Random_Unit();
  return attenuation * Renderer::Instance()->Ray_Color(Ray(rec.position, reflected_direction), rec.hit_counts);
}
