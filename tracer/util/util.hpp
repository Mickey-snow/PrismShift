#ifndef COMMON_H
#define COMMON_H

#include<cassert>

// Headers
#include "vector3.hpp"
#include "vector2.hpp"
#include "color.hpp"
#include "ray.hpp"
#include "interval.hpp"
#include "constant.hpp"
#include "random.hpp"
#include "mat.hpp"
#include "aabb.hpp"

// Utility Functions
inline double Degree2Rad(const double& degrees){
  return degrees * pi / 180.0;
}

#endif
