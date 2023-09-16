#ifndef UTIL_TRANSFORM_H
#define UTIL_TRANSFORM_H

#include "matrix.hpp"

class Point3;
class Vector3;
class Normal;
class Ray;

class Transformation{
public:
  Transformation() : m(Matrix4::I()), minv(Matrix4::I()) {}
  Transformation(const Matrix4& _m) : m(_m), minv(Matrix4::Inverse(_m)) {}
  Transformation(const Matrix4& _m, const Matrix4 _minv) : m(_m),minv(_minv) {}

  Matrix4 Matrix(void) const { return m; }
  Matrix4 InvMatrix(void) const { return minv; }
  
  static Transformation Inverse(const Transformation& t){ return Transformation(t.InvMatrix(), t.Matrix()); }
  Transformation Inverse(void) const { return Transformation(minv,m); }

  bool operator == (const Transformation& rhs) const { return m==rhs.Matrix() && minv==rhs.InvMatrix(); }
  bool operator != (const Transformation& rhs) const { return !(*this == rhs); }

  Transformation operator * (const Transformation& rhs) const { return Transformation(m*rhs.m, rhs.minv*minv); }
  Transformation& operator *= (const Transformation& rhs){ return *this = *this * rhs; }
  
  bool isIdentity(void) const { return m==Matrix4::I(); }

  Point3 operator () (const Point3&) const;
  Vector3 operator () (const Vector3&) const;
  Normal operator () (const Normal&) const;
  Ray operator () (const Ray&) const;
  
private:
  Matrix4 m,minv;



public:
  static Transformation Translate(const Vector3& p);
  static Transformation Translate(const double& dx, const double& dy, const double& dz);

  static Transformation Rotate(Vector3 axis, const double& costheta, const double& sintheta);
  static Transformation Rotate(Vector3 axis, const double& theta);
  static Transformation RotateX(const double& theta);
  static Transformation RotateX(const double& costheta, const double& sintheta);
  static Transformation RotateY(const double& theta);
  static Transformation RotateY(const double& costheta, const double& sintheta);
  static Transformation RotateZ(const double& theta);
  static Transformation RotateZ(const double& costheta, const double& sintheta);
  static Transformation RotateFrTo(const Vector3& fr, const Vector3& to);

  static Transformation Scale(const Vector3& n);
  static Transformation Scale(const Vector3& n, const double& k);
};

#endif
