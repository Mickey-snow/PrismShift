#pragma once

#include "shape.hpp"
#include "util/transform.hpp"

#include <memory>

class Material;
struct HitRecord;
template <typename>
class Interval;
class Ray;
class BSDF;
class AABB;

class Primitive {
 public:
  explicit Primitive(std::shared_ptr<IShape> shape,
                     std::shared_ptr<Material> mat,
                     std::shared_ptr<ITransformation> trans = nullptr);

  HitRecord Hit(Ray r, Interval<double> t) const;
  AABB GetBbox(void) const;

 private:
  std::shared_ptr<IShape> shape_;
  std::shared_ptr<Material> material_;
  std::shared_ptr<ITransformation> transform_;
  AABB bbox_;  // transformed bbox
};
