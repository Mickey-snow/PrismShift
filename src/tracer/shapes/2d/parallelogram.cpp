#include "parallelogram.hpp"

#include <util/util.hpp>

#include <cmath>

AABB Parallelogram::GetBbox() const {
  static const AABB box = AABB(Point3(0, 0, 0), Point3(1, 0, 1)).Pad();
  return box;
}

inline static auto OnObject(double a, double b) {
  return 0 <= a && a <= 1 && 0 <= b && b <= 1;
}

HitRecord Parallelogram::Hit(const Ray& r,
                             const Interval<double>& time_interval) const {
  const double time = -r.Origin().y() / r.Direction().y();
  if (std::isnan(time))
    return HitRecord();
  if (!time_interval.Surrounds(time))
    return HitRecord();

  auto hit_point = r.At(time);
  if (!OnObject(hit_point.x(), hit_point.z()))
    return HitRecord();

  static const Normal normal{0, 1, 0};
  return HitRecord::RTN(r, time, normal);
}
