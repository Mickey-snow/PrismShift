#include "parallelogram.hpp"

#include <util/util.hpp>

#include <cmath>

AABB Parallelogram::GetBbox() const {
  static const AABB box = AABB(Point3(0, 0, 0), Point3(1, 1, 1)).Pad();
  return box;
}

HitRecord Parallelogram::Hit(const Ray& r,
                              const Interval<double>& time_interval) const {
  static auto On_Object = [](const double& a, const double& b) {
    return 0 <= a && a <= 1 && 0 <= b && b <= 1;
  };

  double time = -r.Origin().z() / r.Direction().z();
  if (std::isnan(time))
    return HitRecord();
  if (!time_interval.Surrounds(time))
    return HitRecord();

  auto hit_point = r.At(time);
  if (!On_Object(hit_point.x(), hit_point.y()))
    return HitRecord();

  static const Normal normal{0, 0, 1};
  return HitRecord::RTN(r, time, normal);
}
