#include "ray.hpp"
#include "geometry.hpp"
#include "transform.hpp"

#include <cmath>

Point3 Ray::At(const double& time) const { return origin + time * direction; }

Ray Ray::Transform(const ITransformation& tr) const {
  return Ray(tr.Doit(Origin()), tr.Doit(Direction()));
}

Ray Ray::UndoTransform(const ITransformation& tr) const {
  return Ray(tr.Undo(Origin()), tr.Undo(Direction()));
}
