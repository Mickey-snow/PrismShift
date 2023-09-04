#include "metal.hpp"

#include<renderer.hpp>
#include<util/util.hpp>

Color Metal::Ray_Color(const Ray& r,const Hit_record& rec) const {
  Vector3 reflected_direction = Ray::Reflect_Direction(Vector3::Unit_vector(r.Direction()), rec.normal);
  reflected_direction = reflected_direction + fuzz*Vector3::Random_Unit();
  auto attenuation = texture->ColorAt(rec.hitted_obj->Map_Texture(r, rec));
  return attenuation * Renderer::Instance()->Ray_Color(Ray(rec.position, reflected_direction), rec.hit_counts);
}
