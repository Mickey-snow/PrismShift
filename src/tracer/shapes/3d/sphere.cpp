#include "sphere.hpp"

#include <cmath>
#include <memory>

#include <shape.hpp>
#include <util/util.hpp>

Sphere::Sphere(Point3 o, double r)
    : trans_(std::make_shared<VectorScale>(r, r, r),
             std::make_shared<VectorTranslate>(Vector3(o))),
      area_(4 * pi * r * r),
      bbox_(o + Vector3(r, r, r), o - Vector3(r, r, r)) {}

AABB Sphere::GetBbox() const { return bbox_; }

HitRecord Sphere::Hit(const Ray& ray,
                      const Interval<double>& time_interval) const {
  constexpr double radius = 1.0;
  Ray r = ray.UndoTransform(trans_);

  Vector3 oc = (Vector3)r.Origin();
  auto a = r.Direction().Length_squared();
  auto half_b = Vector3::Dot(oc, r.Direction());
  auto c = oc.Length_squared() - radius * radius;

  auto discriminant = half_b * half_b - a * c;
  if (discriminant < 0)
    return HitRecord();
  auto sqrtd = std::sqrt(discriminant);

  // Find the nearest root that lies in the acceptable range.
  auto root = (-half_b - sqrtd) / a;
  if (!time_interval.Surrounds(root)) {
    root = (-half_b + sqrtd) / a;
    if (!time_interval.Surrounds(root))
      return HitRecord();
  }

  double time = root;
  Point3 position = r.At(time);
  Normal normal = Normal(position);

  return HitRecord::RTN(ray, time, trans_.Doit(normal));
}

ShapeSample Sphere::Sample() const {
  ShapeSample sample;
  sample.pdf = 1.0 / area_;
  sample.pos = Point3(0, 0, 0) + rand_sphere_uniform();
  sample.normal = Normal(sample.pos);
  sample.pos = trans_.Doit(sample.pos);
  sample.normal = trans_.Doit(sample.normal);
  return sample;
}

double Sphere::Area() const { return area_; }
