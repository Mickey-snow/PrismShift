#pragma once

#include "shape.hpp"
#include "util/color.hpp"
#include "util/transform.hpp"

#include <memory>

struct Material;
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
                     std::shared_ptr<ILight> light);

  HitRecord Hit(Ray r, Interval<double> t) const;
  AABB GetBbox(void) const;
  Color Le(Ray r) const;

  [[nodiscard]] inline std::shared_ptr<Material> GetMaterial() const {
    return material_;
  }
  [[nodiscard]] inline std::shared_ptr<ILight> GetLight() const {
    return light_;
  }
  [[nodiscard]] inline std::shared_ptr<IShape> GetShape() const {
    return shape_;
  }

  std::shared_ptr<IShape> shape_;
  std::shared_ptr<Material> material_;
  std::shared_ptr<ILight> light_;
};

template <typename T, typename... Ts>
inline std::shared_ptr<Primitive> MakePrimitive(std::shared_ptr<Material> mat,
                                                std::shared_ptr<ILight> light,
                                                Ts&&... params) {
  auto shape = std::make_shared<T>(std::forward<Ts>(params)...);
  return std::make_shared<Primitive>(std::move(shape), std::move(mat),
                                     std::move(light));
}
