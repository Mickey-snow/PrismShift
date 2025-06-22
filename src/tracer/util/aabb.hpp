#pragma once

#include "util/vector_fwd.hpp"
#include "util/interval.hpp"
#include "util/transform.hpp"

#include <cmath>
#include <ostream>

struct Ray;

class AABB {
  // Approximate an visible object to a cube
  // All rays miss this cube thus miss the object
 public:
  explicit constexpr AABB()
      : x_interval(Interval<Float>::Empty),
        y_interval(Interval<Float>::Empty),
        z_interval(Interval<Float>::Empty) {}
  constexpr AABB(Interval<Float> x, Interval<Float> y, Interval<Float> z)
      : x_interval(std::move(x)),
        y_interval(std::move(y)),
        z_interval(std::move(z)) {}

  AABB(std::initializer_list<AABB> li);
  AABB(std::initializer_list<Point3> li);
  AABB(const AABB& a, const AABB& b);
  AABB(const Point3& a, const Point3& b);

  bool operator==(const AABB& rhs) const {
    return x_interval == rhs.Axis(0) && y_interval == rhs.Axis(1) &&
           z_interval == rhs.Axis(2);
  }

  const Interval<Float>& Axis(const int& n) const;

  bool isEmpty() const;
  bool isHitIn(const Ray&, Interval<Float>) const;
  bool Contains(const Point3&) const;
  bool Contains(const AABB&) const;

  AABB Pad() const;

  AABB Transform(const ITransformation&) const;
  AABB UndoTransform(const ITransformation&) const;

  class Componentbased_Comparer {
   public:
    Componentbased_Comparer(const int& _component) : component(_component) {}
    bool operator()(const AABB& a, const AABB& b) const {
      return a.Axis(component).begin < b.Axis(component).begin;
    }

   private:
    int component;
  };

  friend std::ostream& operator<<(std::ostream& os, const AABB& box) {
    os << 'x' << box.x_interval << ' ';
    os << 'y' << box.y_interval << ' ';
    os << 'z' << box.z_interval << ' ';
    return os;
  }

 private:
  Interval<Float> x_interval, y_interval, z_interval;
};

static_assert(Transformable<AABB>);
