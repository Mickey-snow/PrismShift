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
  Float pdf;
};

class IShape {
 public:
  virtual ~IShape() = default;

  virtual HitRecord Hit(const Ray&, const Interval<Float>&) const = 0;
  virtual AABB GetBbox() const = 0;

  virtual Float Area() const;
  virtual ShapeSample Sample() const;

  Float Pdf(Point3 ref, Vector3 wo) const;
};
