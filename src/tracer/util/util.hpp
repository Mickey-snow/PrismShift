#pragma once

#include <cassert>

// Headers
#include "aabb.hpp"
#include "color.hpp"
#include "constant.hpp"
#include "coordinate.hpp"
#include "geometry.hpp"
#include "interval.hpp"
#include "mat.hpp"
#include "matrix.hpp"
#include "random.hpp"
#include "ray.hpp"
#include "sampling.hpp"
#include "transform.hpp"
#include "vecmath.hpp"

// Utility Functions
inline double Degree2Rad(const double& degrees) { return degrees * pi / 180.0; }
inline double Rad2Degree(const double& rad) { return rad / 180.0 * pi; }

template <typename... Ts>
struct overload : Ts... {
  overload(Ts... ts) : Ts(std::move(ts))... {}
  using Ts::operator()...;
};
template <typename... Ts>
overload(Ts...) -> overload<Ts...>;
