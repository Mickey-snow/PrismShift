#pragma once

#include <shape.hpp>

class Triangle : public IShape {
 public:
  Triangle() = default;
  ~Triangle() = default;

  AABB GetBbox() const override;
  HitRecord Hit(const Ray&, const Interval<double>&) const override;
  ShapeSample Sample() const override;
};
