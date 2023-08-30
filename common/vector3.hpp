#ifndef VEC3_H
#define VEC3_H

#include<iostream>

class Vector3{
public:
  Vector3();
  Vector3(const double&,const double&,const double&);
  Vector3(std::initializer_list<double>);

  double x() const;
  double y() const;
  double z() const;

  Vector3 operator -() const;
  Vector3 operator +(const Vector3&) const;
  Vector3& operator += (const Vector3&);
  Vector3 operator -(const Vector3&) const;
  Vector3& operator -= (const Vector3&);
  Vector3& operator *=(const double&);
  Vector3 operator /(const double&) const;
  Vector3& operator /=(const double&);

  double operator[] (const int& i) const;
  double& operator[] (const int& i);

  double Length() const;
  double Length_squared() const;

  bool Near_Zero() const;

  static Vector3 Random_Unit();
  
protected:
  double e[3];
};
using Point3 = Vector3;


Vector3 operator * (const double&, const Vector3&);
Vector3 operator * (const Vector3&, const double&);

double Dot(const Vector3& u,const Vector3& v);
Vector3 Cross(const Vector3& u,const Vector3& v);

Vector3 Unit_vector(const Vector3& v);

std::ostream& operator << (std::ostream& out, const Vector3& v);


#endif
