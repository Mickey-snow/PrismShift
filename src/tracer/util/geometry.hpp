#ifndef UTILITY_GEOMETRY_H
#define UTILITY_GEOMETRY_H

#include<iostream>

class Vector3{
public:
  Vector3(){ v[0]=v[1]=v[2]=0; }
  Vector3(const double& e1,const double& e2,const double& e3){ v[0]=e1; v[1]=e2; v[2]=e3; }
  Vector3(std::initializer_list<double> li){ v[0]=li[0]; v[1]=li[1]; v[2]=li[2]; }

  // data access
  double& x() noexcept{ return v[0]; }
  double& y() noexcept{ return v[1]; }
  double& z() noexcept{ return v[2]; }
  double x() noexcept const{ return v[0]; }
  double y() noexcept const{ return v[1]; }
  double z() noexcept const{ return v[2]; }
  double operator[] (const int& i) const{ return v[i]; }
  double& operator[] (const int& i){ return v[i]; }

  Vector3 operator -() const { return Vector3{-v[0],-v[1],-v[2]}; }
  Vector3 operator +(const Vector3& rhs) const { return Vector3{v[0]+rhs[0], v[1]+rhs[1], v[2]+rhs[2]}; }
  Vector3& operator += (const Vector3& rhs){ return *this = *this + rhs; }
  Vector3 operator -(const Vector3& rhs) const { return *this + (-rhs); }
  Vector3& operator -= (const Vector3& rhs){ return *this = *this + (-rhs); }
  Vector3 operator *(const double& rhs){ return Vector3{v[0]*rhs, v[1]*rhs, v[2]*rhs}; }
  Vector3& operator *=(const double& rhs){ return *this = *this * rhs; }
  Vector3 operator /(const double& rhs) const{ return Vector3{v[0]/rhs, v[1]/rhs, v[2]/rhs}; }
  Vector3& operator /=(const double& rhs){ return *this = *this / rhs; }


  double Length() const;
  double Length_squared() const;

  Vector3 Unit() const;

  bool Near_Zero() const;

  static Vector3 Random_Unit();
  static Vector3 Random(const double& min=-1.0, const double& max=1.0);

  static double Dot(const Vector3& u,const Vector3& v);
  static Vector3 Cross(const Vector3& u,const Vector3& v);

  static Vector3 Unit_vector(const Vector3& v);

protected:
  double v[3];
};



class Vector2{
public:
  Vector2(){ v[0] = v[1] = 0.0; }
  Vector2(const double& x, const double& y){ v[0]=x; v[1]=y; }
  Vector2(std::initializer_list<double> li){ v[0]=li[0]; v[1]=li[1]; }

  // data access methods
  double const x() const noexcept { return v[0]; }
  double& x() noexcept { return v[0]; }
  double const y() const noexcept { return v[1]; }
  double& y() noexcept { return v[1]; }
  double operator [](const int& i) const{ return v[i]; }
  double& operator [](const int& i){ return v[i]; }

  // arithmetics
  Vector2 operator -() const{ return Vector2{-v[0],-v[1]}; }
  Vector2 operator + (const Vector2& rhs) const { return Vector2{v[0]+rhs[0],v[1]+rhs[1]}; }
  Vector2& operator += (const Vector2& rhs){ return *this = *this + rhs; }
  Vector2 operator - (const Vector2& rhs) const { return *this + (-rhs); }
  Vector2& operator -= (const Vector2& rhs){ return *this = *this - rhs; }
  Vector2 operator * (const double& rhs) const { return Vector2{v[0]*rhs,v[1]*rhs}; }
  Vector2& operator *= (const double& rhs){ return *this = *this * rhs; }
  Vector2 operator / (const double& rhs) const { return Vector2{v[0]/rhs, v[1]/rhs}; }
  Vector2& operator /= (const double& rhs) const { return *this = *this / rhs; }
  static double Dot(const Vector2& lhs, const Vector2& rhs){ return lhs[0]*rhs[0]+lhs[1]*rhs[1]; }
  double Dot(const Vector2& rhs) const { return Vector2::Dot(*this, rhs); }
  static Vector3 Cross(const Vector2& lhs, const Vector2& rhs);
  Vector3 Cross(const Vector2& rhs) const;

  
  double v[2];
};

#endif
