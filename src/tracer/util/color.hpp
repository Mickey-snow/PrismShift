#ifndef COLOR_H
#define COLOR_H

#include<iostream>
#include "geometry.hpp"

class Color : public internal::BaseVec3<Color>{
private:
  using Parent = internal::BaseVec3<Color>;
public:
  using Parent::Parent;

  template<typename T>
  Color operator * (const BaseVec3<T>& rhs) const{
    return Color(v[0]*rhs.v[0], v[1]*rhs.v[1], v[2]*rhs.v[2]);
  }
  template<typename T>
  Color& operator *= (const BaseVec3<T>& rhs){ return *this = *this * rhs; }

  template<typename T>
  Color operator / (const BaseVec3<T>& rhs) const{
    return Color(v[0]/rhs.v[0], v[1]/rhs.v[1], v[2]/rhs.v[2]);
  }
  template<typename T>
  Color& operator /= (const BaseVec3<T>& rhs){ return *this = *this / rhs; }

  Color operator * (const double& rhs)const{ return Color(v[0]*rhs, v[1]*rhs, v[2]*rhs); }
  friend Color operator * (const double& lhs, const Color& rhs){ return rhs * lhs; }
  Color operator / (const double& rhs) const { return Color(v[0]/rhs, v[1]/rhs, v[2]/rhs); }


  template<typename T>
  Color operator + (const BaseVec3<T>& rhs) const{ return Color(v[0]+rhs.v[0], v[1]+rhs.v[1], v[2]+rhs.v[2]); }
  template<typename T>
  Color& operator += (const BaseVec3<T>& rhs){ return *this = *this + rhs; }
};


Color Format_Color(const Color& pixel_color,const double& scale=255.0);

#endif
