#ifndef COMMON_H
#define COMMON_H

#include<cassert>

// Headers
#include "geometry.hpp"
#include "matrix.hpp"
#include "transform.hpp"
#include "coordinate.hpp"
#include "color.hpp"
#include "ray.hpp"
#include "interval.hpp"
#include "constant.hpp"
#include "random.hpp"
#include "mat.hpp"
#include "aabb.hpp"
#include "sampling.hpp"

// Utility Functions
inline double Degree2Rad(const double& degrees){
  return degrees * pi / 180.0;
}
inline double Rad2Degree(const double& rad){
  return rad / 180.0 * pi;
}

#endif
