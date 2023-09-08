#include "metal.hpp"
#include<shape.hpp>

#include<renderer.hpp>
#include<util/util.hpp>

Color Metal::Ray_Color(const Hit_record& rec) const {
  Vector3 reflected_direction = rec.ray.Reflect_Direction(rec.normal);
  reflected_direction = reflected_direction + fuzz*Vector3::Random_Unit();
  auto attenuation = texture->ColorAt(rec.hitted_obj->Map_Texture(rec));
  return attenuation * Renderer::Instance()->Ray_Color(Ray(rec.position, reflected_direction), rec.hit_counts);
}
