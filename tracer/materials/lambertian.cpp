#include "lambertian.hpp"

#include<util/util.hpp>
#include<renderer.hpp>

Color Lambertian::Ray_Color(const Hit_record& rec) const {
  Vector3 scattered_direction = rec.ray.Scatter_Direction(rec.normal);
  auto attenuation = texture->ColorAt(rec.hitted_obj->Map_Texture(rec));
  return attenuation * Renderer::Instance()->Ray_Color(Ray(rec.position, scattered_direction), rec.hit_counts);
}
