#include "shape.hpp"

Float IShape::Area() const { return 0; }

ShapeSample IShape::Sample() const {
  ShapeSample sample;
  sample.pdf = 0;
  return sample;
}

Float IShape::Pdf(Point3 ref, Vector3 wo) const {
  const Float area = Area();
  if (area <= 0)
    return 0;

  HitRecord rec = Hit(Ray(ref, wo), Interval<Float>::Positive());
  if (!rec.hits)
    return 0;

  const Float d2 = (rec.position - ref).Length_squared();
  const Float cos = std::fabs(Vector3::Dot(rec.normal, wo));

  return (1.0 / area) * d2 / cos;
}
