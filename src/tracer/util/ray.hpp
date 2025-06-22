#pragma once

#include "util/transform.hpp"
#include "util/vector.hpp"

#include <memory>
#include <ostream>

struct Ray {
  constexpr Ray() {}
  constexpr Ray(Point3 origin_, Vector3 direction_)
      : origin(std::move(origin_)), direction(std::move(direction_)) {}

  Point3 const Origin() const { return origin; }
  Vector3 const Direction() const { return direction; }
  Ray& SetOrigin(Point3 orig) {
    origin = std::move(orig);
    return *this;
  }
  Ray& SetDirection(Vector3 dir) {
    direction = std::move(dir);
    return *this;
  }

  Point3 At(const Float&) const;

  Ray Transform(const ITransformation&) const;
  Ray UndoTransform(const ITransformation&) const;

  bool operator==(const Ray& rhs) const {
    return origin == rhs.Origin() && direction == rhs.Direction();
  }
  bool operator!=(const Ray& rhs) const { return !(*this == rhs); }

  friend std::ostream& operator<<(std::ostream& os, const Ray& r) {
    os << "O" << r.Origin() << " d" << r.Direction();
    return os;
  }

  Point3 origin;
  Vector3 direction;
};

static_assert(Transformable<Ray>);
