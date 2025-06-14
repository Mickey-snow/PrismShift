#pragma once

#include "shape.hpp"
#include "util/transform.hpp"

#include <memory>

class Material;
struct HitRecord;
template <typename>
class Interval;
struct Ray;
class BSDF;
class ILight;
class AABB;

class Primitive {
 public:
  explicit Primitive(std::shared_ptr<IShape> shape,
                     std::shared_ptr<Material> mat,
                     std::shared_ptr<ILight> light,
                     std::shared_ptr<ITransformation> trans = nullptr);

  HitRecord Hit(Ray r, Interval<double> t) const;
  AABB GetBbox(void) const;
  std::shared_ptr<ITransformation> GetTransform() const;
  [[nodiscard]] inline std::shared_ptr<Material> GetMaterial() const {
    return material_;
  }
  [[nodiscard]] inline std::shared_ptr<ILight> GetLight() const {
    return light_;
  }

 private:
  std::shared_ptr<IShape> shape_;
  std::shared_ptr<Material> material_;
  std::shared_ptr<ILight> light_;
  std::shared_ptr<ITransformation> transform_;
  AABB bbox_;  // transformed bbox
};
