#ifndef UTIL_TRANSFORM_H
#define UTIL_TRANSFORM_H

#include<utility>
#include<ostream>

#include "util/geometry_fwd.hpp"
#include "matrix.hpp"


class Ray;
class AABB;


/**
 * @interface ITransformation
 * @brief Interface for geometric transformations.
 *
 * This interface defines the contract for classes that implement geometric transformations.
 * It requires the implementation of methods to transform the three basic geometric primitives:
 * points, vectors, and normals. Each transformation class inheriting from this interface
 * must provide its own logic for how these primitives are transformed.
 *
 * The interface also defines template methods for `Doit` and `Undo`, which
 * allow for transformation of any type that implements a `Transform` method compatible
 * with the `ITransformation` interface.
 */
class ITransformation{
public:
  virtual ~ITransformation() = default;

  virtual Point3 Doit(const Point3&) const = 0;
  virtual Vector3 Doit(const Vector3&) const = 0;
  virtual Normal Doit(const Normal&) const = 0;
  template<typename T>
  T Doit(const T& t) const{ return t.Transform(*this); }

  virtual Point3 Undo(const Point3&) const = 0;
  virtual Vector3 Undo(const Vector3&) const = 0;
  virtual Normal Undo(const Normal&) const = 0;
  template<typename T>
  T Undo(const T& t) const{ return t.Transform(*this); }
};


/**
 * @brief Implements translating transformations.
 *
 * Provides functionality for translate transformations along the three axes (x, y, z).
 */
class VectorTranslate : public ITransformation{
private:
  double dx,dy,dz;

public:
  VectorTranslate() : dx(0.0), dy(0.0), dz(0.0) {}
  VectorTranslate(const double& ix, const double& iy, const double& iz) : dx(ix), dy(iy), dz(iz) {}
  VectorTranslate(const Vector3& v);

  Point3 Doit(const Point3&) const override;
  Vector3 Doit(const Vector3&) const override;
  Normal Doit(const Normal&) const override;
  Point3 Undo(const Point3&) const override;
  Vector3 Undo(const Vector3&) const override;
  Normal Undo(const Normal&) const override;

public:
  static VectorTranslate Translate(const double& x, const double& y, const double& z){
    return VectorTranslate(x,y,z);
  }
  static VectorTranslate Translate(const Vector3& v){
    return VectorTranslate(v);
  }
};



/**
 * @brief Implements scaling transformations.
 *
 * Provides functionality for scaling transformations along the three axes (x, y, z).
 */
class VectorScale : public ITransformation{
private:
  double dx,dy,dz;
public:
  VectorScale() : dx(1.0), dy(1.0), dz(1.0) {}
  VectorScale(const double& ix, const double& iy, const double& iz) : dx(ix),dy(iy),dz(iz) {}
  VectorScale(const Vector3& v);

  Point3 Doit(const Point3&) const override;
  Vector3 Doit(const Vector3&) const override;
  Normal Doit(const Normal&) const override;
  Point3 Undo(const Point3&) const override;
  Vector3 Undo(const Vector3&) const override;
  Normal Undo(const Normal&) const override;

public:
  static VectorScale Scale(const double& x, const double& y, const double& z){
    return VectorScale(x,y,z);
  }
  static VectorScale Scale(const Vector3& v){
    return VectorScale(v);
  }
};



/**
 * @class MatrixTransformation
 * @brief Implements the ITransformation interface for geometric transformations.
 *
 * This class provides functionality to transform geometric entities
 * using a 4x4 homogeneous transformation matrix.
 *
 * Example:
 *   Matrix4 matrix; // Assume this is initialized
 *   Transformation transform(matrix);
 *   Point3 point(1.0, 2.0, 3.0);
 *   Point3 transformedPoint = transform.Doit(point);
 *
 * Notes:
 *   - The class handles the inverse of the transformation matrix internally
 *     to provide efficient inverse transformations.
 *   - The transformation of normals is handled differently to maintain their
 *     perpendicularity to the tangent plane of the surface after transformation.
 */
class Transformation : public ITransformation{
public:
  Transformation() : m(Matrix4::I()), minv(Matrix4::I()) {}
  Transformation(const Matrix4& _m) : m(_m), minv(_m.inv()) {}
  Transformation(const Matrix4& _m, const Matrix4 _minv) : m(_m),minv(_minv) {}

  Matrix4 const Matrix(void) const { return m; }
  Matrix4 const InvMatrix(void) const { return minv; }

  Point3 Doit(const Point3& it) const override;
  Vector3 Doit(const Vector3& it) const override;
  Normal Doit(const Normal& it) const override;
  Point3 Undo(const Point3& it) const override;
  Vector3 Undo(const Vector3& it) const override;
  Normal Undo(const Normal& it) const override;
  
  
  static Transformation Inverse(const Transformation& t){ return Transformation(t.InvMatrix(), t.Matrix()); }
  Transformation Inverse(void) const { return Transformation(minv,m); }

  bool operator == (const Transformation& rhs) const { return m==rhs.Matrix() && minv==rhs.InvMatrix(); }
  bool operator != (const Transformation& rhs) const { return !(*this == rhs); }

  Transformation operator * (const Transformation& rhs) const { return Transformation(m*rhs.m, rhs.minv*minv); }
  Transformation& operator *= (const Transformation& rhs){ return *this = *this * rhs; }
  
  bool isIdentity(void) const { return m==Matrix4::I(); }

  [[deprecated]]
  Point3 operator () (const Point3&) const;
  [[deprecated]]
  Vector3 operator () (const Vector3&) const;
  [[deprecated]]
  Normal operator () (const Normal&) const;

  template<typename T> [[deprecated]]
  T operator () (const T& t) const{ return t.Transform(*this); }


  friend std::ostream& operator << (std::ostream& os, const Transformation& tr){
    os << "m=" << tr.m << " minv=" << tr.minv;
    return os;
  }
  
private:
  Matrix4 m,minv;

  // factory methods for creating the transformation matrix
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

  static Transformation Scale(const double&, const double&, const double&);
  static Transformation Scale(const Vector3& n);
  static Transformation Scale(const Vector3& n, const double& k);
};
using MatrixTransformation = Transformation;

#endif
