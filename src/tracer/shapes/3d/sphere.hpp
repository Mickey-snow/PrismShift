#pragma once

#include "shape.hpp"
#include "util/transform.hpp"

class Sphere : public IShape {
 public:
  Sphere(Point3 o, Float r);

  Point2 GetUv(Point3 p) const;

  AABB GetBbox(void) const override;
  HitRecord Hit(const Ray& r, const Interval<Float>& time) const override;
  ShapeSample Sample() const override;
  Float Area() const override;

  // for testing
  inline auto GetTransformation() const { return trans_; }
  inline auto r() const noexcept { return r_; }

 private:
  VectorTranslate trans_;
  Float r_;
  AABB bbox_;
};
