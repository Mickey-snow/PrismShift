#include "geometry.hpp"

#include<cmath>

#include "random.hpp"
#include "transform.hpp"

Point2 Point2::operator + (const Vector2& rhs) const{
  return Point2(v[0]+rhs.v[0], v[1]+rhs.v[1]);
}

Vector2 Point2::operator - (const Point2& rhs) const {
  return Vector2(v[0]-rhs.v[0], v[1]-rhs.v[1]);
}

Point2 Point2::operator - (const Vector2& rhs) const{
  return Point2(v[0]-rhs.v[0], v[1]-rhs.v[1]);
}

Vector3 Vector3::Normalized(const Vector3& v){ return v.Normalize(); }

Vector3 Vector3::Random_Unit(void){
  double x,y,z,pi=std::numbers::pi;
  auto r1 = random_uniform_01(), r2 = random_uniform_01();
  x = cos(2*pi*r1)*2*std::sqrt(r2*(1-r2));
  y = sin(2*pi*r1)*2*std::sqrt(r2*(1-r2));
  z = 1 - 2*r2;
  return Vector3{x,y,z};
}


Vector3 Vector3::Transform(const Transformation& tr) const{ return tr(*this); }
Point3 Point3::Transform(const Transformation& tr) const{ return tr(*this); }
Normal Normal::Transform(const Transformation& tr) const{ return tr(*this); }


Vector3 Vector3::Random(const double& min, const double& max){
  return Vector3{random_double(min,max), random_double(min,max), random_double(min,max)};
}
