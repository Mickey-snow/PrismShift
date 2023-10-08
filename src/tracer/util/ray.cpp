#include "geometry.hpp"
#include "transform.hpp"
#include "ray.hpp"

#include<cmath>

Point3 Ray::At(const double& time) const{
  return origin + time*direction;
}


Ray Ray::Transform(const Transformation& tr) const{
  Ray ret = *this;
  ret.SetOrigin(tr(Origin()));
  ret.SetDirection(tr(Direction()));
  return ret;
}
