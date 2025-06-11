#pragma once

#include <cstddef>

template <std::size_t>
class Vector;

using Vector2 = Vector<2>;
using Vector3 = Vector<3>;

template <std::size_t>
class Point;

using Point2 = Point<2>;
using Point3 = Point<3>;

class Normal;
