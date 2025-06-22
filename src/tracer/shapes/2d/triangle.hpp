#pragma once

#include <shape.hpp>

class Triangle : public IShape {
 public:
  Triangle(Point3 o, Point3 a, Point3 b);

  AABB GetBbox() const override;
  HitRecord Hit(const Ray&, const Interval<Float>&) const override;
  ShapeSample Sample() const override;
  Float Area() const override;

 private:
  MatrixTransformation trans_;
  Float area_;
};
