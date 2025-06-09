#include "sphere.hpp"

#include <cmath>
#include <memory>

#include <shape.hpp>
#include <util/util.hpp>

AABB Sphere::Get_Bbox(void) const {
  static AABB bbox = AABB(Point3(1, 1, 1), Point3(-1, -1, -1));
  return bbox;
}

Hit_record Sphere::Hit(const Ray& r,
                       const Interval<double>& time_interval) const {
  constexpr double radius = 1.0;

  Vector3 oc = (Vector3)r.Origin();
  auto a = r.Direction().Length_squared();
  auto half_b = Vector3::Dot(oc, r.Direction());
  auto c = oc.Length_squared() - radius * radius;

  auto discriminant = half_b * half_b - a * c;
  if (discriminant < 0)
    return Hit_record{};
  auto sqrtd = std::sqrt(discriminant);

  // Find the nearest root that lies in the acceptable range.
  auto root = (-half_b - sqrtd) / a;
  if (!time_interval.Surrounds(root)) {
    root = (-half_b + sqrtd) / a;
    if (!time_interval.Surrounds(root))
      return Hit_record{};
  }

  double time = root;
  Point3 position = r.At(time);
  Normal normal = (Normal)position;

  return Hit_record::RTN(r, time, normal);
}
