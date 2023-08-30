#include "vector3.hpp"
#include "ray.hpp"

Point3 Ray::At(const double& time) const{
  return origin + time*direction;
}
