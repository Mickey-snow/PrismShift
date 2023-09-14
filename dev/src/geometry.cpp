#include "geometry.hpp"

#include<cmath>

Point2 Point2::operator + (const Vector2& rhs) const{
  return Point2(v[0]+rhs.v[0], v[1]+rhs.v[1]);
}

Vector2 Point2::operator - (const Point2& rhs) const {
  return Vector2(v[0]-rhs.v[0], v[1]-rhs.v[1]);
}

Point2 Point2::operator - (const Vector2& rhs) const{
  return Point2(v[0]-rhs.v[0], v[1]-rhs.v[1]);
}
