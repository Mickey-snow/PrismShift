#include "primitive.hpp"

#include "bsdf.hpp"
#include "light.hpp"
#include "material.hpp"
#include "shape.hpp"
#include "util/util.hpp"

#include "spdlog/spdlog.h"

Primitive::Primitive(std::shared_ptr<IShape> shape,
                     std::shared_ptr<Material> mat,
                     std::shared_ptr<ILight> light,
                     std::shared_ptr<ITransformation> trans)
    : shape_(shape),
      material_(mat),
      light_(light),
      transform_(trans ? trans : identity_transform),
      bbox_(shape->GetBbox().Transform(*transform_)) {}

HitRecord Primitive::Hit(Ray ray, Interval<double> t) const {
  Ray r = ray.UndoTransform(*transform_);  // to shape local coordinate system

  auto rec = shape_->Hit(r, t);
  if (rec.hits) {
    rec.primitive = this;
    rec.position = transform_->Doit(rec.position);
    rec.normal = transform_->Doit(rec.normal);
  }
  return rec;
}

AABB Primitive::GetBbox(void) const { return bbox_; }

Color Primitive::Le(Ray r) const {
  if (!light_)
    return Color(0);
  Ray local = r.Transform(*transform_);
  return light_->Le(local);
}

ShapeSample Primitive::Sample() const {
  ShapeSample sample = shape_->Sample();
  sample.pos = transform_->Doit(sample.pos);
  sample.normal = transform_->Doit(sample.normal);

  // pdf should be (1.0 / A)
  if (sample.pdf > 0 && area_ > 0)
    sample.pdf = 1.0 / area_;
  else
    sample.pdf = 0;

  return sample;
}

double Primitive::Pdf(Point3 ref, Vector3 wo) const {
  if (!light_ || area_ < 0)
    return 0;

  HitRecord rec = Hit(Ray(ref, wo), Interval<double>::Positive());
  if (!rec.hits)
    return 0;

  const double d2 = (rec.position - ref).Length_squared();
  const double cos_light = std::fabs(Vector3::Dot(rec.normal, -wo));

  return (1.0 / area_) * d2 / cos_light;
}
