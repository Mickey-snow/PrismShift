#pragma once

#include "shape.hpp"
#include "util/transform.hpp"

class Sphere : public IShape {
 public:
  Sphere(Point3 o, double d);

  AABB GetBbox(void) const override;
  HitRecord Hit(const Ray& r, const Interval<double>& time) const override;
  ShapeSample Sample() const override;
  double Area() const override;

 private:
  CompositeTransformation trans_;
  double area_;
};
