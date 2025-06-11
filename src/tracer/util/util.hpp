#pragma once

#include <cassert>

// Headers
#include "aabb.hpp"
#include "color.hpp"
#include "constant.hpp"
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
inline constexpr auto Degree2Rad(std::floating_point auto degrees) {
  return degrees * pi / 180.0;
}
inline constexpr auto Rad2Degree(std::floating_point auto rad) {
  return rad / 180.0 * pi;
}

template <typename... Ts>
struct overload : Ts... {
  overload(Ts... ts) : Ts(std::move(ts))... {}
  using Ts::operator()...;
};
template <typename... Ts>
overload(Ts...) -> overload<Ts...>;
