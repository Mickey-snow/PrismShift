#pragma once

#include <shape.hpp>

class AABB;
class Ray;
template <typename>
class Interval;

class Plane : public IShape {
 public:
  Plane() = default;
  ~Plane() = default;

  AABB GetBbox(void) const override;
  HitRecord Hit(const Ray&, const Interval<double>&) const override;
};
