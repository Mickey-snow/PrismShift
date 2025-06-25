#include "primitive.hpp"

#include "bsdf.hpp"
#include "light.hpp"
#include "material.hpp"
#include "shape.hpp"
#include "util/util.hpp"

#include "spdlog/spdlog.h"

Primitive::Primitive(std::shared_ptr<IShape> shape,
                     std::shared_ptr<IMaterial> mat,
                     std::shared_ptr<ILight> light)
    : shape_(shape), material_(mat), light_(light) {}

HitRecord Primitive::Hit(Ray ray, Interval<Float> t) const {
  auto rec = shape_->Hit(ray, t);
  if (rec.hits) {
    rec.primitive = this;
  }
  return rec;
}

AABB Primitive::GetBbox(void) const { return shape_->GetBbox(); }

Color Primitive::Le(Ray r) const {
  if (!light_)
    return Color(0);
  return light_->Le(r);
}
