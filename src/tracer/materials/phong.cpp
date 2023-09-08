#include "phong.hpp"
#include<renderer.hpp>
#include<util/util.hpp>

#include<cmath>

Color Phong::Ray_Color(const Hit_record& rec) const {
  Color col = (rec.hit_counts==1 ? ambient : Color(0,0,0)) + emission;
  Color idiffuse=Color(0,0,0), ispecular=Color(0,0,0);

  if(random_uniform_01() < 0.5){
  Vector3 scattered_direction = rec.ray.Scatter_Direction(rec.normal);
  idiffuse = diffuse * fabs(Vector3::Dot(rec.normal, scattered_direction)) * Renderer::Instance()->Ray_Color(Ray(rec.position, scattered_direction), rec.hit_counts);
  } else {
  Vector3 reflected_direction = rec.ray.Reflect_Direction(rec.normal);
  ispecular = specular * pow(fabs(Vector3::Dot(rec.ray.Direction().Unit(), reflected_direction)), shininess) * Renderer::Instance()->Ray_Color(Ray(rec.position, reflected_direction), rec.hit_counts);
  }
  return col + idiffuse + ispecular;
}
