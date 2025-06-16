#include "shape.hpp"

double IShape::Area() const { return 0; }

ShapeSample IShape::Sample() const {
  ShapeSample sample;
  sample.pdf = 0;
  return sample;
}

double IShape::Pdf(Point3 ref, Vector3 wo) const {
  const double area = Area();
  if (area <= 0)
    return 0;

  HitRecord rec = Hit(Ray(ref, wo), Interval<double>::Positive());
  if (!rec.hits)
    return 0;

  const double d2 = (rec.position - ref).Length_squared();
  const double cos = std::fabs(Vector3::Dot(rec.normal, wo));

  return (1.0 / area) * d2 / cos;
}
