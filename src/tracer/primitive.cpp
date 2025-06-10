#include "primitive.hpp"

#include "bsdf.hpp"
#include "material.hpp"
#include "shape.hpp"
#include "util/util.hpp"

Primitive::Primitive(std::shared_ptr<IShape> shape,
                     std::shared_ptr<IMaterial> mat)
    : shape_(shape), material_(mat) {}

Primitive::~Primitive() = default;

HitRecord Primitive::Hit(const Ray& r, const Interval<double>& t) const {
  if (shape_ == nullptr)
    return HitRecord();

  auto rec = shape_->Hit(r, t);
  if (rec.hits) {
    rec.hitted_obj = this;
    rec.material = material_.get();
  }
  return rec;
}

AABB Primitive::GetBbox(void) const {
  if (shape_ == nullptr)
    return AABB();
  else
    return shape_->GetBbox();
}
