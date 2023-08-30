#ifndef RAY_H
#define RAY_H

#include "vector3.hpp"

class Ray{
public:
  Ray(){}
  Ray(const Point3& origin_, const Vector3& direction_) : origin(origin_), direction(direction_) {}

  Point3 Origin() const{ return origin; }
  Vector3 Direction() const{ return direction; }

  Point3 At(const double&) const;

  static Vector3 Scatter_Direction(const Vector3& normal);
  static Vector3 Reflect_Direction(const Vector3& in_direction, const Vector3& normal);
private:
  Point3 origin;
  Vector3 direction;
};

#endif
