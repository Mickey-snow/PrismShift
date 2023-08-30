#ifndef COMMON_H
#define COMMON_H

#include<cassert>
#include<cmath>
#include<climits>
#include<memory>

// Constants
constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;

// Utility Functions
inline double Degree2Rad(const double& degrees){
  return degrees * pi / 180.0;
}

// Headers
#include "objects.hpp"
#include "vector3.hpp"
#include "color.hpp"
#include "ray.hpp"
#include "interval.hpp"

#endif
