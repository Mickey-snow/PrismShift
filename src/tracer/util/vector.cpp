#include "util/vector.hpp"

#include <cmath>

#include "random.hpp"
#include "transform.hpp"

template <>
Vector<3> Vector<3>::Random_Unit(void) {
  double x, y, z, pi = std::numbers::pi;
  auto r1 = random_uniform_01(), r2 = random_uniform_01();
  x = cos(2 * pi * r1) * 2 * std::sqrt(r2 * (1 - r2));
  y = sin(2 * pi * r1) * 2 * std::sqrt(r2 * (1 - r2));
  z = 1 - 2 * r2;
  return Vector<3>{x, y, z};
}
template <>
Vector<3> Vector<3>::Random(const double& min, const double& max) {
  return Vector<3>{random_double(min, max), random_double(min, max),
                   random_double(min, max)};
}

template <>
Vector<3> Vector<3>::Transform(const ITransformation& tr) const {
  return tr.Doit(*this);
}
template <>
Point<3> Point<3>::Transform(const ITransformation& tr) const {
  return tr.Doit(*this);
}
Normal Normal::Transform(const ITransformation& tr) const {
  return tr.Doit(*this);
}
