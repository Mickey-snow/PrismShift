#include "triangle.hpp"

#include <util/util.hpp>

#include <cmath>

using namespace vec_helpers;

Triangle::Triangle(Point3 o, Point3 a, Point3 b)
    : trans_(MatrixTransformation::ChangeCoordinate(o, a, b)) {
  Vector3 e1 = a - o, e2 = b - o;
  area_ = IsParallel(e1, e2) ? 0.0 : Vector3::Cross(e1, e2).Length() * 0.5;
}

AABB Triangle::GetBbox() const {
  static const AABB box = AABB(Point3(0, 0, 0), Point3(1, 0, 1)).Pad();
  return box.Transform(trans_);
}

inline static auto OnObject(Float a, Float b) {
  return (a + b) <= 1 && 0 <= a && 0 <= b;
}

HitRecord Triangle::Hit(const Ray& ray,
                        const Interval<Float>& time_interval) const {
  Ray r = ray.UndoTransform(trans_);

  Float time = -r.Origin().y() / r.Direction().y();
  if (std::isnan(time))
    return HitRecord();
  if (!time_interval.Surrounds(time))
    return HitRecord();

  auto hit_point = r.At(time);
  if (!OnObject(hit_point.x(), hit_point.z()))
    return HitRecord();

  static const Normal normal{0, 1, 0};
  return HitRecord::Create(time, ray.At(time),
                           Point2(hit_point.x(), hit_point.z()),
                           trans_.Doit(normal));
}

ShapeSample Triangle::Sample() const {
  ShapeSample sample;
  sample.pdf = 1.0 / area_;
  Float a = random_uniform_01(), b = random_uniform_01();
  if (!OnObject(a, b)) {
    a = 1 - a;
    b = 1 - b;
  }
  sample.pos = trans_.Doit(Point3(a, 0, b));
  sample.normal = trans_.Doit(Normal(0, 1, 0));
  return sample;
}

Float Triangle::Area() const { return area_; }
