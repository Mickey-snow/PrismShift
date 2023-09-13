#ifndef UTIL_TRANSFORM_H
#define UTIL_TRANSFORM_H

#include "matrix.hpp"

class Point3;
class Vector3;
class Normal;

class Transform{
public:
  Transform() : m(Matrix4::I()), minv(Matrix4::I()) {}
  Transform(const Matrix4& _m) : m(_m), minv(Matrix4::Inverse(_m)) {}
  Transform(const Matrix4& _m, const Matrix4 _minv) : m(_m),minv(_minv) {}

  Matrix4 Matrix(void) const { return m; }
  Matrix4 InvMatrix(void) const { return minv; }
  
  static Transform Inverse(const Transform& t){ return Transform(t.InvMatrix(), t.Matrix()); }
  Transform Inverse(void) const { return Transform(minv,m); }

  bool operator == (const Transform& rhs) const { return m==rhs.Matrix() && minv==rhs.InvMatrix(); }
  bool operator != (const Transform& rhs) const { return !(*this == rhs); }

  bool isIdentity(void) const { return m==Matrix4::I(); }

  Point3 operator () (const Point3&) const;
  Vector3 operator () (const Vector3&) const;
  Normal operator () (const Normal&) const;
  
private:
  Matrix4 m,minv;
};

#endif
