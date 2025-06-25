#pragma once

#include <shape.hpp>

struct Ray;
class AABB;
template <typename>
class Interval;

class Parallelogram : public IShape {
 public:
  Parallelogram(Point3 o, Point3 a, Point3 b);

  AABB GetBbox() const override;
  HitRecord Hit(const Ray&, const Interval<Float>&) const override;
  ShapeSample Sample() const override;
  Float Area() const override;

 private:
  MatrixTransformation trans_;
  Float area_;
};
