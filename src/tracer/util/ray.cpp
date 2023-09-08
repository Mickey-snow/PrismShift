#include "vector3.hpp"
#include "ray.hpp"

#include<cmath>

Point3 Ray::At(const double& time) const{
  return origin + time*direction;
}

Vector3 Ray::Scatter_Direction(const Vector3& normal) const{
  Vector3 scattered = normal + Vector3::Random_Unit();
  return scattered.Near_Zero() ? normal : scattered;
}

Vector3 Ray::Reflect_Direction(const Vector3& normal) const{
  Vector3 direction = Vector3::Unit_vector(this->direction);
  Vector3 scaled_normal = normal * (-Vector3::Dot(direction, normal));
  return direction + 2*scaled_normal;
}

Vector3 Ray::Refract_Direction(const Vector3& normal, const double& refractive_index_ratio) const{
  Vector3 direction = Vector3::Unit_vector(this->direction);
  double cos_theta = fmin(Vector3::Dot(-direction, normal), 1.0);
  Vector3 out_direction_perpendicular = refractive_index_ratio * (direction + cos_theta*normal);
  Vector3 out_direction_parallel = -std::sqrt(fabs(1.0-out_direction_perpendicular.Length_squared())) * normal;
  return out_direction_parallel + out_direction_perpendicular;
}


double Ray::intersectionTimeWithPlane(const Point3& Q, const Vector3 &u, const Vector3 &v)const{
  double tup,tdown;
  Vector3 normal = Vector3::Unit_vector(Vector3::Cross(u,v));
  tup = Vector3::Dot(normal, Q) - Vector3::Dot(origin, normal);
  tdown = Vector3::Dot(direction, normal);
  return tup/tdown;
}
