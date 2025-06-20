#pragma once

#include "shape.hpp"
#include "util/transform.hpp"

class Sphere : public IShape {
 public:
  Sphere(Point3 o, double r);

  Point2 GetUv(Point3 p) const;

  AABB GetBbox(void) const override;
  HitRecord Hit(const Ray& r, const Interval<double>& time) const override;
  ShapeSample Sample() const override;
  double Area() const override;

  // for testing
  inline auto GetTransformation() const { return trans_; }
  inline auto r() const noexcept { return r_; }

 private:
  VectorTranslate trans_;
  double r_;
  AABB bbox_;
};
