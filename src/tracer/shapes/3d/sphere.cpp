#include "sphere.hpp"

#include <cassert>
#include <cmath>
#include <memory>

#include <shape.hpp>
#include <util/util.hpp>
#include "spdlog/spdlog.h"

Sphere::Sphere(Point3 o, Float r)
    : trans_(o - Point3(0, 0, 0)),
      r_(r),
      bbox_(o + Vector3(r, r, r), o - Vector3(r, r, r)) {}

AABB Sphere::GetBbox() const { return bbox_; }

Point2 Sphere::GetUv(Point3 p) const {
#ifndef NDEBUG
  if (std::abs((p - Point3(0, 0, 0)).Length() - r_) >= 1e-6)
    spdlog::warn("Sphere::GetUv: r_={}, p=({},{},{}), lenp={}", r_, p.x(),
                 p.y(), p.z(), p.Length());
#endif

  Float phi = std::atan2(p.z(), p.x());
  Float cos0 = std::clamp<Float>(p.y() / r_, -1, 1);
  Float theta = std::acos(cos0);
  return Point2(phi / (2 * pi) + 0.5, theta / pi);
}

HitRecord Sphere::Hit(const Ray& ray,
                      const Interval<Float>& time_interval) const {
  Ray r = ray.UndoTransform(trans_);

  Vector3 oc = (Vector3)r.Origin();
  Float a = r.Direction().Length_squared();
  Float b = Vector3::Dot(oc, r.Direction()) * 2;
  Float c = oc.Length_squared() - r_ * r_;

  Float discriminant = b * b - 4 * a * c;
  if (discriminant < 0)
    return HitRecord();
  Float sqrtd = std::sqrt(discriminant);

  // Find the nearest root that lies in the acceptable range.
  Float root = (-b - sqrtd) / (2 * a);
  if (!time_interval.Surrounds(root)) {
    root = (-b + sqrtd) / (2 * a);
    if (!time_interval.Surrounds(root))
      return HitRecord();
  }

  Float time = root;
  Point3 position = r.At(time);
  Point2 uv = GetUv(position);
  Normal normal = Normal(position);

  position = ray.At(time);
  normal = trans_.Doit(normal);
  return HitRecord::Create(time, position, uv, normal);
}

ShapeSample Sphere::Sample() const {
  ShapeSample sample;
  sample.pdf = 1.0 / Area();
  sample.pos = Point3(0, 0, 0) + rand_sphere_uniform();
  sample.normal = Normal(sample.pos);
  sample.pos = trans_.Doit(sample.pos);
  sample.normal = trans_.Doit(sample.normal);
  return sample;
}

Float Sphere::Area() const { return 4 * pi * Sqr(r_); }
