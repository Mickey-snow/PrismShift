#pragma once

#include "shape.hpp"

#include <memory>

class IMaterial;
struct HitRecord;
template <typename>
class Interval;
class Ray;
class BSDF;
class AABB;

class IPrimitive : public IShape {
 public:
  virtual ~IPrimitive() = default;

  // Hit_record Hit(const Ray&, const Interval<double>&) const
  // AABB GetBbox(void) const
  // methods for geometry calculation

  virtual BSDF CalcBSDF(const HitRecord&) const = 0;
  // methods for calculate scattering distribution
};

class Primitive {
 public:
  explicit Primitive(std::shared_ptr<IShape> shape,
                     std::shared_ptr<IMaterial> mat);
  ~Primitive();

  HitRecord Hit(const Ray& r, const Interval<double>& t) const;
  AABB GetBbox(void) const;

 private:
  std::shared_ptr<IShape> shape_;
  std::shared_ptr<IMaterial> material_;
};
