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
  Ray ray = r.Transform(*transform_);

  auto rec = shape_->Hit(ray, t);
  if (rec.hits) {
    rec.primitive = this;
    rec.material = material_.get();
    // Note: in the future, material properties should be computed using local
    // coordinates

    rec.ray = r;
    rec.normal = transform_->Undo(rec.front_face ? rec.normal : -rec.normal);
    rec.position = transform_->Undo(rec.position);
  }
  return rec;
}

AABB Primitive::GetBbox(void) const { return bbox_; }
