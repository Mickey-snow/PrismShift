#pragma once

#include <shape.hpp>

class Triangle : public IShape {
 public:
  Triangle(Point3 o, Point3 a, Point3 b);

  AABB GetBbox() const override;
  HitRecord Hit(const Ray&, const Interval<double>&) const override;
  ShapeSample Sample() const override;
  double Area() const override;

 private:
  MatrixTransformation trans_;
  double area_;
};
