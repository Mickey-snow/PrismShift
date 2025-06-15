#pragma once

#include <shape.hpp>

class Sphere : public IShape {
 public:
  Sphere() = default;
  ~Sphere() = default;

  AABB GetBbox(void) const override;
  HitRecord Hit(const Ray& r, const Interval<double>& time) const override;
  ShapeSample Sample() const override;
};
