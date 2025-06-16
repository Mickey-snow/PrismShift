#include "sphere.hpp"

#include <cmath>
#include <memory>

#include <shape.hpp>
#include <util/util.hpp>

Sphere::Sphere(Point3 o, double d)
    : trans_(std::make_shared<VectorScale>(d, d, d),
             std::make_shared<VectorTranslate>(Vector3(o))) {
  d /= 2.0;
  area_ = 4 * pi * d;
}

AABB Sphere::GetBbox(void) const {
  static AABB bbox = AABB(Point3(1, 1, 1), Point3(-1, -1, -1));
  return bbox.Transform(trans_);
}

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
  Normal normal = (Normal)position;

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
