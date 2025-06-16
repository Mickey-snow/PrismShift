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
  HitRecord Hit(const Ray&, const Interval<double>&) const override;
  ShapeSample Sample() const override;
  double Area() const override;

 private:
  MatrixTransformation trans_;
  double area_;
};
