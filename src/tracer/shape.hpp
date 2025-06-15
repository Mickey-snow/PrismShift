#pragma once

#include "hit_record.hpp"
#include "util/aabb.hpp"
#include "util/ray.hpp"
#include "util/transform.hpp"
#include "util/vector.hpp"

template <typename T>
class Interval;
class Primitive;

struct ShapeSample {
  Point3 pos;
  Normal normal;
  double pdf;
};

class IShape {
 public:
  virtual ~IShape() = default;

  virtual HitRecord Hit(const Ray&, const Interval<double>&) const = 0;
  virtual AABB GetBbox(void) const = 0;
  virtual ShapeSample Sample() const {
    ShapeSample sample;
    sample.pdf = 0;
    return sample;
  }
};
