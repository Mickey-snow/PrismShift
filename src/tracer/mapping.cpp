#include "mapping.hpp"

#include "util/util.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

SphericalMap::SphericalMap(std::shared_ptr<ITransformation> trans)
    : trans_(trans) {}

Point2 SphericalMap::Map(Point3 p) const {
  Point3 pt = trans_->Undo(p);
  double r = pt.Length();
  assert(r > 0);
  double phi = std::atan2(pt.z(), pt.x());
  double cos0 = std::clamp<double>(pt.y() / r, -1, 1);
  double theta = std::acos(cos0);
  return Point2(phi / (2 * pi) + 0.5, theta / pi);
}
