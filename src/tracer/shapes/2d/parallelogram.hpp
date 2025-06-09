#pragma once

#include <shape.hpp>

class Ray;
class AABB;
template <typename>
class Interval;

class Parallelogram : public IShape {
 public:
  Parallelogram() = default;
  ~Parallelogram() = default;

  AABB Get_Bbox() const override;
  Hit_record Hit(const Ray&, const Interval<double>&) const override;
};
