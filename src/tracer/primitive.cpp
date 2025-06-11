#include "primitive.hpp"

#include "bsdf.hpp"
#include "material.hpp"
#include "shape.hpp"
#include "util/util.hpp"

Primitive::Primitive(std::shared_ptr<IShape> shape,
                     std::shared_ptr<Material> mat,
                     std::shared_ptr<ITransformation> trans)
    : shape_(shape),
      material_(mat),
      transform_(trans ? trans : identity_transform),
      bbox_(shape->GetBbox().Transform(*transform_)) {}

HitRecord Primitive::Hit(Ray r, Interval<double> t) const {
  r = r.UndoTransform(*transform_);  // to shape local coordinate system

  auto rec = shape_->Hit(r, t);
  if (rec.hits) {
    rec.primitive = this;
    rec.material = material_.get();
  }
  return rec;
}

AABB Primitive::GetBbox(void) const { return bbox_; }

std::shared_ptr<ITransformation> Primitive::GetTransform() const {
  return transform_;
}
