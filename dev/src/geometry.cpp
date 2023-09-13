#include "geometry.hpp"

#include<cmath>

namespace develop{
  constexpr double EPS = 1e-6;

  Vector3::Vector3(const Point3& p){ v[0]=p.x();v[1]=p.y();v[2]=p.z(); }
  Vector3::Vector3(const Normal& p){ v[0]=p.x();v[1]=p.y();v[2]=p.z(); }
  Point2::Point2(const Point3& p){ v[0]=p.v[0]; v[1]=p.v[1]; }
  
  double Vector3::Length_squared(const Vector3& v){ return v[0]*v[0]+v[1]*v[1]+v[2]*v[2]; }
  double Vector3::Length(const Vector3& v){ return std::sqrt(Vector3::Length_squared(v)); }
  double Vector3::Length_squared(void) const { return Vector3::Length_squared(*this); }
  double Vector3::Length(void) const{ return Vector3::Length(*this); }
  double Vector2::Length_squared(const Vector2& v){ return v[0]*v[0]+v[1]*v[1]; }
  double Vector2::Length(const Vector2& v){ return std::sqrt(Vector2::Length_squared(v)); }
  double Vector2::Length_squared(void) const { return Vector2::Length_squared(*this); }
  double Vector2::Length(void) const{ return Vector2::Length(*this); }
  
  
  Vector3 Vector3::Normalize(const Vector3& v){ return v / v.Length(); }
  Vector3& Vector3::Normalize(void){ return *this = Vector3::Normalize(*this); }
  Vector2 Vector2::Normalize(const Vector2& v){ return v / v.Length(); }
  Vector2& Vector2::Normalize(void){ return *this = Vector2::Normalize(*this); }
  
  bool Vector3::operator == (const Vector3& rhs) const {
    return fabs(v[0]-rhs[0])<EPS && fabs(v[1]-rhs[1])<EPS && fabs(v[2]-rhs[2])<EPS;
  }
  bool Vector3::operator != (const Vector3& rhs) const { return !(*this == rhs); }
  bool Vector3::Near_Zero() const{ return *this == Vector3(0,0,0); }
  bool Vector2::operator == (const Vector2& rhs) const {
    return fabs(v[0]-rhs[0])<EPS && fabs(v[1]-rhs[1])<EPS;
  }
  bool Vector2::operator != (const Vector2& rhs) const { return !(*this == rhs); }
  bool Vector2::Near_Zero() const{ return *this == Vector2(0,0); }

  
  bool Vector3::hasnans(void) const{ return std::isnan(v[0])||std::isnan(v[1])||std::isnan(v[2]); }
  bool Vector3::isvalid(void) const{ return !hasnans(); }
  bool Vector2::hasnans(void) const{ return std::isnan(v[0])||std::isnan(v[1]); }
  bool Vector2::isvalid(void) const{ return !hasnans(); }



  bool Point2::operator == (const Point2& rhs) const {
    return fabs(v[0]-rhs[0])<EPS && fabs(v[1]-rhs[1])<EPS;
  }
  bool Point2::operator != (const Point2& rhs) const { return !(*this == rhs); }
  bool Point2::hasnans(void) const{ return std::isnan(v[0])||std::isnan(v[1]); }
  bool Point2::isvalid(void) const{ return !hasnans(); }
  bool Point3::operator == (const Point3& rhs) const {
    return fabs(v[0]-rhs[0])<EPS && fabs(v[1]-rhs[1])<EPS && fabs(v[2]-rhs[2])<EPS; 
  }
  bool Point3::operator != (const Point3& rhs) const { return !(*this == rhs); }
  bool Point3::hasnans(void) const{ return std::isnan(v[0])||std::isnan(v[1])||std::isnan(v[2]); }
  bool Point3::isvalid(void) const{ return !hasnans(); }


  double Vector3::Dot(const Vector3& u,const Vector3& v){
    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
  }
  Vector3 Vector3::Cross(const Vector3& u,const Vector3& v){
  return Vector3(u[1] * v[2] - u[2] * v[1],
		 u[2] * v[0] - u[0] * v[2],
		 u[0] * v[1] - u[1] * v[0]);
  }
  double Vector3::Dot(const Vector3& v) const { return Vector3::Dot(*this, v); }
  Vector3& Vector3::Cross(const Vector3& v){ return *this = Vector3::Cross(*this, v); }

  Vector3 Vector2::Cross(const Vector2& u,const Vector2& v){
    return Vector3(0,0,u[0]*v[1]-u[1]*v[0]);
  }
  Vector3 Vector2::Cross(const Vector2& v) const { return Vector2::Cross(*this, v); }
}
