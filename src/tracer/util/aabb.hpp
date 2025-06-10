#pragma once

#include <cmath>
#include <ostream>

#include "util/geometry_fwd.hpp"
#include "util/interval.hpp"

class Ray;
class MatrixTransformation;

class AABB {
  // Approximate an visible object to a cube
  // All rays miss this cube thus miss the object
 public:
  explicit AABB()
      : x_interval(Interval<double>::Empty()),
        y_interval(Interval<double>::Empty()),
        z_interval(Interval<double>::Empty()) {}
  AABB(Interval<double> x, Interval<double> y, Interval<double> z)
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

  const Interval<double>& Axis(const int& n) const;

  bool isEmpty() const;
  bool isHitIn(const Ray&, Interval<double>) const;
  bool Contains(const Point3&) const;
  bool Contains(const AABB&) const;

  AABB Pad() const;

  AABB Transform(const MatrixTransformation&) const;

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
  Interval<double> x_interval, y_interval, z_interval;
};
