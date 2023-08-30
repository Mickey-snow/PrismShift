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
  
private:
  Point3 origin;
  Vector3 direction;
};

#endif
