#pragma once

#include <shape.hpp>

class AABB;
class Ray;
template <typename>
class Interval;

class Triangle : public IShape {
 public:
  Triangle() = default;
  ~Triangle() = default;

  AABB GetBbox() const override;
  HitRecord Hit(const Ray&, const Interval<double>&) const override;
};
