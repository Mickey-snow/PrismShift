#include "plane.hpp"

#include <util/util.hpp>

AABB Plane::GetBbox(void) const {
  static const AABB box = AABB(Interval<double>::Universe(), Interval<double>(),
                               Interval<double>::Universe())
                              .Pad();
  return box;
}

HitRecord Plane::Hit(const Ray& r,
                     const Interval<double>& time_interval) const {
  double time = -r.Origin().y() / r.Direction().y();
  if (std::isnan(time))
    return HitRecord();
  if (!time_interval.Contains(time))
    return HitRecord();

  static const Normal normal{0, 1, 0};
  return HitRecord::RTN(r, time, normal);
}
