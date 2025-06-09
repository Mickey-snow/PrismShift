#include "ray.hpp"
#include "geometry.hpp"
#include "transform.hpp"

#include <cmath>

Point3 Ray::At(const double& time) const { return origin + time * direction; }

Ray Ray::Transform(const ITransformation& tr) const {
  Ray ret = *this;
  ret.SetOrigin(tr.Doit(Origin()));
  ret.SetDirection(tr.Doit(Direction()));
  return ret;
}
