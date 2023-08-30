#include "vector3.hpp"
#include "ray.hpp"

Point3 Ray::At(const double& time) const{
  return origin + time*direction;
}

Vector3 Ray::Scatter_Direction(const Vector3& normal){
  Vector3 scattered = normal + Vector3::Random_Unit();
  return scattered.Near_Zero() ? normal : scattered;
}

Vector3 Ray::Reflect_Direction(const Vector3& in_direction, const Vector3& normal){
  Vector3 scaled_normal = normal * (-Dot(in_direction, normal));
  return in_direction + 2*scaled_normal;
}
