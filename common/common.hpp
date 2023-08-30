#ifndef COMMON_H
#define COMMON_H

#include<cassert>
#include<cmath>
#include<memory>

// Headers
#include "vector3.hpp"
#include "color.hpp"
#include "ray.hpp"
#include "interval.hpp"
#include "constant.hpp"

// Utility Functions
inline double Degree2Rad(const double& degrees){
  return degrees * pi / 180.0;
}

#endif