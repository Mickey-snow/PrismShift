#include "vector3.hpp"
#include "ray.hpp"

#include<cmath>

Point3 Ray::At(const double& time) const{
  return origin + time*direction;
}

Vector3 Ray::Scatter_Direction(const Vector3& normal){
  Vector3 scattered = normal + Vector3::Random_Unit();
  return scattered.Near_Zero() ? normal : scattered;
}

Vector3 Ray::Reflect_Direction(const Vector3& in_direction, const Vector3& normal){
  Vector3 scaled_normal = normal * (-Vector3::Dot(in_direction, normal));
  return in_direction + 2*scaled_normal;
}

Vector3 Ray::Refract_Direction(const Vector3& in_direction, const Vector3& normal, const double& refractive_index_ratio){
  double cos_theta = fmin(Vector3::Dot(-in_direction, normal), 1.0);
  Vector3 out_direction_perpendicular = refractive_index_ratio * (in_direction + cos_theta*normal);
  Vector3 out_direction_parallel = -std::sqrt(fabs(1.0-out_direction_perpendicular.Length_squared())) * normal;
  return out_direction_parallel + out_direction_perpendicular;
}
