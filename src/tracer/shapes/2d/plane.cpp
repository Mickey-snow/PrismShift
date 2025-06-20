#include "plane.hpp"

#include <util/util.hpp>

Plane ::Plane(Point3 o, Point3 a, Point3 b)
    : trans_(MatrixTransformation::ChangeCoordinate(o, a, b)) {}

AABB Plane::GetBbox(void) const {
  static const AABB box = AABB(Interval<double>::Universe(), Interval<double>(),
                               Interval<double>::Universe())
                              .Pad();
  return box.Transform(trans_);
}

HitRecord Plane::Hit(const Ray& ray,
                     const Interval<double>& time_interval) const {
  Ray r = ray.UndoTransform(trans_);

  double time = -r.Origin().y() / r.Direction().y();
  if (std::isnan(time))
    return HitRecord();
  if (!time_interval.Contains(time))
    return HitRecord();

  Point3 hit_point = r.At(time);
  double u = hit_point.x();
  double v = hit_point.z();

  static const Normal normal{0, 1, 0};
  return HitRecord::Create(time, ray.At(time),
                           Point2(u - std::floor(u), v - std::floor(v)),
                           trans_.Doit(normal));
}
