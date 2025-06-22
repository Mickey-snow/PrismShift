#pragma once

#include "util/prshdefs.hpp"

#include <limits>
#include <numbers>

// Constants
constexpr Float infinity = std::numeric_limits<Float>::infinity();
inline constexpr Float pi = std::numbers::pi_v<Float>;
constexpr Float invpi = static_cast<Float>(1.0) / pi;
