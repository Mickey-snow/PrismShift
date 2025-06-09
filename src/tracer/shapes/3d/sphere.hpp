#pragma once

#include <shape.hpp>

#include <cmath>
#include <memory>
#include <sstream>
#include <string>

class AABB;
class Hit_record;
class Ray;
template <typename>
class Interval;

class Sphere : public IShape {
 public:
  Sphere() = default;
  ~Sphere() = default;

  AABB Get_Bbox(void) const override;
  Hit_record Hit(const Ray& r, const Interval<double>& time) const override;
};
