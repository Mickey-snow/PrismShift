#pragma once

#include "shape.hpp"

class Plane : public IShape {
 public:
  Plane(Point3 o, Point3 a, Point3 b);

  AABB GetBbox(void) const override;
  HitRecord Hit(const Ray& ray,
                const Interval<Float>& interval) const override;

 private:
  MatrixTransformation trans_;
};
