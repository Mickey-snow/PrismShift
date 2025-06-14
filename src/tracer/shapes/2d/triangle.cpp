#include "triangle.hpp"

#include <util/util.hpp>

#include <cmath>

AABB Triangle::GetBbox() const {
  static const AABB box = AABB(Point3(0, 0, 0), Point3(1, 0, 1)).Pad();
  return box;
}

inline static auto OnObject(double a, double b) {
  return (a + b) <= 1 && 0 <= a && 0 <= b;
}

HitRecord Triangle::Hit(const Ray& r,
                        const Interval<double>& time_interval) const {
  double time = -r.Origin().y() / r.Direction().y();
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
