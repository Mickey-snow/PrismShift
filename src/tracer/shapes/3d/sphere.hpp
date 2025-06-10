#pragma once

#include <shape.hpp>

#include <cmath>
#include <memory>
#include <sstream>
#include <string>

class AABB;
class HitRecord;
class Ray;
template <typename>
class Interval;

class Sphere : public IShape {
 public:
  Sphere() = default;
  ~Sphere() = default;

  AABB GetBbox(void) const override;
  HitRecord Hit(const Ray& r, const Interval<double>& time) const override;
};
