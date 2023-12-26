#ifndef UTIL_TRANSFORM_H
#define UTIL_TRANSFORM_H

#include<utility>
#include<ostream>

#include "util/geometry_fwd.hpp"
#include "matrix.hpp"


/**
 * @interface ITransformation
 *
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
class MatrixTransformation : public ITransformation{
public:
  MatrixTransformation() : m(Matrix4::I()), minv(Matrix4::I()) {}
  MatrixTransformation(const Matrix4& _m) : m(_m), minv(_m.inv()) {}
  MatrixTransformation(const Matrix4& _m, const Matrix4 _minv) : m(_m),minv(_minv) {}

  Matrix4 const Matrix(void) const { return m; }
  Matrix4 const InvMatrix(void) const { return minv; }

  Point3 Doit(const Point3& it) const override;
  Vector3 Doit(const Vector3& it) const override;
  Normal Doit(const Normal& it) const override;
  Point3 Undo(const Point3& it) const override;
  Vector3 Undo(const Vector3& it) const override;
  Normal Undo(const Normal& it) const override;
  
  
  static MatrixTransformation Inverse(const MatrixTransformation& t){ return MatrixTransformation(t.InvMatrix(), t.Matrix()); }
  MatrixTransformation Inverse(void) const { return MatrixTransformation(minv,m); }

  bool operator == (const MatrixTransformation& rhs) const { return m==rhs.Matrix() && minv==rhs.InvMatrix(); }
  bool operator != (const MatrixTransformation& rhs) const { return !(*this == rhs); }

  MatrixTransformation operator * (const MatrixTransformation& rhs) const { return MatrixTransformation(m*rhs.m, rhs.minv*minv); }
  MatrixTransformation& operator *= (const MatrixTransformation& rhs){ return *this = *this * rhs; }
  
  bool isIdentity(void) const { return m==Matrix4::I(); }

  [[deprecated]]
  Point3 operator () (const Point3&) const;
  [[deprecated]]
  Vector3 operator () (const Vector3&) const;
  [[deprecated]]
  Normal operator () (const Normal&) const;
  template<typename T> [[deprecated]]
  T operator () (const T& t) const{ return t.Transform(*this); }


  friend std::ostream& operator << (std::ostream& os, const MatrixTransformation& tr){
    os << "m=" << tr.m << " minv=" << tr.minv;
    return os;
  }
  
private:
  Matrix4 m,minv;

  // factory methods for creating the transformation matrix
public:			 
  static MatrixTransformation Translate(const Vector3& p);
  static MatrixTransformation Translate(const double& dx, const double& dy, const double& dz);

  static MatrixTransformation Rotate(basic_vector<double,3> axis, const double& costheta, const double& sintheta);
  static MatrixTransformation Rotate(const basic_vector<double,3>& axis, const double& theta);
  static MatrixTransformation RotateX(const double& theta);
  static MatrixTransformation RotateX(const double& costheta, const double& sintheta);
  static MatrixTransformation RotateY(const double& theta);
  static MatrixTransformation RotateY(const double& costheta, const double& sintheta);
  static MatrixTransformation RotateZ(const double& theta);
  static MatrixTransformation RotateZ(const double& costheta, const double& sintheta);
  static MatrixTransformation RotateFrTo(const Vector3& fr, const Vector3& to);
  /**
   * @brief Creates a transformation matrix that aligns the global coordinate system to a given local coordinate system.
   *
   * @param The basis of the local coordinate system.
   *
   * @return MatrixTransformation representing the rotation.
   */
  static MatrixTransformation AlignXYZ(const basic_vector<Vector3,3>&);
  static MatrixTransformation AlignXYZ(const Vector3&,const Vector3&,const Vector3&);
  
  static MatrixTransformation Scale(const double&, const double&, const double&);
  static MatrixTransformation Scale(const Vector3& n);
  static MatrixTransformation Scale(const Vector3& n, const double& k);
};



class Quaternion{
public:
  Quaternion(const double& is, const double& ix, const double& iy, const double& iz) :
    s(is), v(ix,iy,iz) {}
  Quaternion(const double& is, const basic_vector<double,3>& iv):
    s(is), v(iv) {}

  ~Quaternion() = default;
    
  Quaternion operator + (const Quaternion& rhs) const;
  Quaternion& operator +=(const Quaternion& rhs){ return *this = *this + rhs; }
  Quaternion operator - (const Quaternion& rhs) const;
  Quaternion& operator -=(const Quaternion& rhs){ return *this = *this - rhs; }

  Quaternion operator * (const Quaternion& rhs) const;
  Quaternion& operator *=(const Quaternion& rhs){ return *this = *this * rhs; }
  Quaternion operator * (const double& r) const;
  Quaternion& operator *=(const double& r){ return *this = *this * r; }
  friend Quaternion operator *(const double& r, const Quaternion& rhs){
    return rhs*r;
  }
  Quaternion operator / (const double& r) const;
  Quaternion& operator /=(const double& r){ return *this = *this / r; }

  Quaternion conj(void) const;

  double sqrnorm(void) const;
  double norm(void) const;

  double Dot(const Quaternion&) const;

  Quaternion inv(void) const;
  
public:
  double s;
  Vector3 v;
};


class QuaternionTransform : public ITransformation{
public:
  QuaternionTransform(const Quaternion& quat) : q(quat), qinv(quat.inv()) {}
  QuaternionTransform(const Quaternion& quat, const Quaternion& quatinv) :
    q(quat), qinv(quatinv) {}
  ~QuaternionTransform() = default;
  
  Point3 Doit(const Point3&) const override;
  Point3 Undo(const Point3&) const override;
  Vector3 Doit(const Vector3&) const override;
  Vector3 Undo(const Vector3&) const override;
  Normal Doit(const Normal&) const override;
  Normal Undo(const Normal&) const override;
  
private:
  Quaternion q,qinv;

  basic_vector<double,3> Doit_impl(const basic_vector<double,3>&) const;
  basic_vector<double,3> Undo_impl(const basic_vector<double,3>&) const;

public:
  /**
   * @brief creates a quaternion representing a rotation.
   * 
   * @param A 3D vector representing the axis of rotation.
   * @param The angle of rotation in radians. The rotation is applied counter-clockwise
   * when looking along the direction of the axis vector.
   * 
   * @return QuaternionTransform The resulting quaternion encapsulates the rotation.
   *
   * Example usage:
   * auto qt = QuaternionTransform::Rotate(Vector3(0, 0, 1), M_PI / 4);
   * // Rotate 45 degrees around the Z-axis
   */
  static QuaternionTransform Rotate(const basic_vector<double,3>&, double);
  static QuaternionTransform RotateFrTo(const Vector3& fr, const Vector3& to);

  /**
   * @brief Creates a quaternion transformation that aligns the global coordinate system to a given local coordinate system.
   *
   * @param The basis of the local coordinate system.
   *
   * @return QuaternionTransform representing the rotation world->local.
   *
   * Note: the basis must form a proper coordinate system.
   */
  static QuaternionTransform AlignXYZ(const basic_vector<Vector3,3>&);
  static QuaternionTransform AlignXYZ(const Vector3&,const Vector3&,const Vector3&);
};

#endif
