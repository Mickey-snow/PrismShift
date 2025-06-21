#pragma once

#include "util/transform.hpp"
#include "util/vector_fwd.hpp"

class SphericalMap {
 public:
  explicit SphericalMap(std::shared_ptr<ITransformation> trans);

  Point2 Map(Point3 p) const;

 private:
  std::shared_ptr<ITransformation> trans_;
};
