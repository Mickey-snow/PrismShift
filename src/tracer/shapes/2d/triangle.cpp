#include "triangle.hpp"

#include <util/util.hpp>

#include <cmath>

AABB Triangle::GetBbox() const {
  static const AABB box = AABB(Point3(0, 0, 0), Point3(1, 1, 0)).Pad();
  return box;
}

HitRecord Triangle::Hit(const Ray& r,
                         const Interval<double>& time_interval) const {
  static auto On_Object = [](const double& a, const double& b) {
    return (a + b) <= 1 && 0 <= a && 0 <= b;
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
