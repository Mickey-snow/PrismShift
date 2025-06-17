#pragma once

#include "shape.hpp"
#include "util/transform.hpp"

class Sphere : public IShape {
 public:
  Sphere(Point3 o, double r);

  AABB GetBbox(void) const override;
  HitRecord Hit(const Ray& r, const Interval<double>& time) const override;
  ShapeSample Sample() const override;
  double Area() const override;

  // for testing
  inline auto GetTransformation() const { return trans_; }

 private:
  CompositeTransformation trans_;
  double area_;
  AABB bbox_;
};
