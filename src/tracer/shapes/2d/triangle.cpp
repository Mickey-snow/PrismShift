#include "triangle.hpp"

#include <util/util.hpp>

#include <cmath>

AABB Triangle::Get_Bbox() const {
  static const AABB box = AABB(Point3(0, 0, 0), Point3(1, 1, 0)).Pad();
  return box;
}

Hit_record Triangle::Hit(const Ray& r,
                         const Interval<double>& time_interval) const {
  static auto On_Object = [](const double& a, const double& b) {
    return (a + b) <= 1 && 0 <= a && 0 <= b;
  };

  double time = -r.Origin().z() / r.Direction().z();
  if (std::isnan(time))
    return Hit_record{};
  if (!time_interval.Surrounds(time))
    return Hit_record{};

  auto hit_point = r.At(time);
  if (!On_Object(hit_point.x(), hit_point.y()))
    return Hit_record{};

  static const Normal normal{0, 0, 1};
  return Hit_record::RTN(r, time, normal);
}
