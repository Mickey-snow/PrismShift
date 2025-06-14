#pragma once

#include "util/matrix.hpp"
#include "util/vecmath.hpp"
#include "util/vector.hpp"

#include <memory>
#include <ostream>
#include <utility>

class ITransformation;
template <class T>
concept Transformable = requires(const T t, const ITransformation& tr) {
  { t.Transform(tr) } -> std::same_as<T>;
  { t.UndoTransform(tr) } -> std::same_as<T>;
};

/**
 * @interface ITransformation
 *
 * @brief Interface for geometric transformations.
 *
 * This interface defines the contract for classes that implement geometric
 * transformations. It requires the implementation of methods to transform the
 * three basic geometric primitives: points, vectors, and normals. Each
 * transformation class inheriting from this interface must provide its own
 * logic for how these primitives are transformed.
 *
 * The interface also defines template methods for `Doit` and `Undo`, which
 * allow for transformation of any type that implements a `Transform` method
 * compatible with the `ITransformation` interface.
 */
class ITransformation {
 public:
  virtual ~ITransformation() = default;

  virtual Point3 Doit(Point3) const = 0;
  virtual Vector3 Doit(Vector3) const = 0;
  virtual Normal Doit(Normal) const = 0;
  inline auto Doit(Transformable auto const& t) const {
    return t.Transform(*this);
  }

  virtual Point3 Undo(Point3) const = 0;
  virtual Vector3 Undo(Vector3) const = 0;
  virtual Normal Undo(Normal) const = 0;
  inline auto Undo(Transformable auto const& t) const {
    return t.UndoTransform(*this);
  }
};
extern std::shared_ptr<ITransformation> const identity_transform;

/**
 * @brief Implements translating transformations.
 *
 * Provides functionality for translate transformations along the three axes (x,
 * y, z).
 */
class VectorTranslate : public ITransformation {
 private:
  double dx, dy, dz;

 public:
  constexpr VectorTranslate() noexcept : dx(0.0), dy(0.0), dz(0.0) {}
  constexpr VectorTranslate(double ix, double iy, double iz) noexcept
      : dx(ix), dy(iy), dz(iz) {}
  VectorTranslate(Vector3 v);

  Point3 Doit(Point3) const override;
  Vector3 Doit(Vector3) const override;
  Normal Doit(Normal) const override;
  Point3 Undo(Point3) const override;
  Vector3 Undo(Vector3) const override;
  Normal Undo(Normal) const override;

 public:
  static VectorTranslate Translate(double x, double y, double z) {
    return VectorTranslate(x, y, z);
  }
  inline static VectorTranslate Translate(Vector3 v) {
    return VectorTranslate(std::move(v));
  }
};

/**
 * @brief Implements scaling transformations.
 *
 * Provides functionality for scaling transformations along the three axes (x,
 * y, z).
 */
class VectorScale : public ITransformation {
 private:
  double dx, dy, dz;

 public:
  constexpr VectorScale() noexcept : dx(1.0), dy(1.0), dz(1.0) {}
  constexpr VectorScale(double ix, double iy, double iz) noexcept
      : dx(ix), dy(iy), dz(iz) {}
  VectorScale(Vector3 v);

  Point3 Doit(Point3) const override;
  Vector3 Doit(Vector3) const override;
  Normal Doit(Normal) const override;
  Point3 Undo(Point3) const override;
  Vector3 Undo(Vector3) const override;
  Normal Undo(Normal) const override;

 public:
  static VectorScale Scale(double x, double y, double z) {
    return VectorScale(x, y, z);
  }
  inline static VectorScale Scale(Vector3 v) {
    return VectorScale(std::move(v));
  }
};

/**
 * @class MatrixTransformation
 * @brief Implements the ITransformation interface for geometric
 * transformations.
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
 *     perpendicularity to the tangent plane of the surface after
 * transformation.
 */
class MatrixTransformation : public ITransformation {
 public:
  MatrixTransformation() : m(Matrix4::I()), minv(Matrix4::I()) {}
  MatrixTransformation(Matrix4 _m) : m(std::move(_m)), minv(m.inv()) {}
  MatrixTransformation(Matrix4 _m, const Matrix4 _minv)
      : m(std::move(_m)), minv(std::move(_minv)) {}

  Matrix4 const Matrix(void) const { return m; }
  Matrix4 const InvMatrix(void) const { return minv; }

  Point3 Doit(Point3 it) const override;
  Vector3 Doit(Vector3 it) const override;
  Normal Doit(Normal it) const override;
  Point3 Undo(Point3 it) const override;
  Vector3 Undo(Vector3 it) const override;
  Normal Undo(Normal it) const override;

  static MatrixTransformation Inverse(const MatrixTransformation& t) {
    return MatrixTransformation(t.InvMatrix(), t.Matrix());
  }
  MatrixTransformation Inverse(void) const {
    return MatrixTransformation(minv, m);
  }

  bool operator==(const MatrixTransformation& rhs) const {
    return m == rhs.Matrix() && minv == rhs.InvMatrix();
  }
  bool operator!=(const MatrixTransformation& rhs) const {
    return !(*this == rhs);
  }

  MatrixTransformation operator*(const MatrixTransformation& rhs) const {
    return MatrixTransformation(m * rhs.m, rhs.minv * minv);
  }
  MatrixTransformation& operator*=(const MatrixTransformation& rhs) {
    return *this = *this * rhs;
  }

  bool isIdentity(void) const { return m == Matrix4::I(); }

  friend std::ostream& operator<<(std::ostream& os,
                                  const MatrixTransformation& tr) {
    os << "m=" << tr.m << " minv=" << tr.minv;
    return os;
  }

 private:
  Matrix4 m, minv;

  // factory methods for creating the transformation matrix
 public:
  static MatrixTransformation Translate(Vector3 p);
  static MatrixTransformation Translate(double dx, double dy, double dz);

  static MatrixTransformation Rotate(basic_vector<double, 3> axis,
                                     double costheta,
                                     double sintheta);
  static MatrixTransformation Rotate(const basic_vector<double, 3>& axis,
                                     double theta);
  static MatrixTransformation RotateX(double theta);
  static MatrixTransformation RotateX(double costheta, double sintheta);
  static MatrixTransformation RotateY(double theta);
  static MatrixTransformation RotateY(double costheta, double sintheta);
  static MatrixTransformation RotateZ(double theta);
  static MatrixTransformation RotateZ(double costheta, double sintheta);
  static MatrixTransformation RotateFrTo(Vector3 fr, Vector3 to);
  /**
   * @brief Creates a transformation matrix that aligns the global coordinate
   * system to a given local coordinate system.
   *
   * @param The basis of the local coordinate system.
   *
   * @return MatrixTransformation representing the rotation.
   */
  static MatrixTransformation AlignXYZ(const basic_vector<Vector3, 3>&);
  static MatrixTransformation AlignXYZ(Vector3, Vector3, Vector3);

  static MatrixTransformation Scale(double, double, double);
  static MatrixTransformation Scale(Vector3 n);
  static MatrixTransformation Scale(Vector3 n, double k);

  
  /// Returns a transformation T such that
  ///   T.Doit(a) == Point3(0,0,0),
  ///   T.Doit(b) == Point3(1,0,0),
  ///   T.Doit(c) == Point3(0,0,1).
  /// We do this by solving for the affine matrix K = [A | t] with
  ///   A*(b–a) = (1,0,0),  A*(c–a) = (0,0,1),  A*(n) = (0,1,0)   (n = plane
  ///   normal)
  /// and  A*a + t = 0.
  static MatrixTransformation TriangleToUnit(Point3 origin, Point3 a, Point3 b);
};

class Quaternion {
 public:
  constexpr Quaternion(double is, double ix, double iy, double iz) noexcept
      : s(is), v(ix, iy, iz) {}
  Quaternion(double is, basic_vector<double, 3> iv) : s(is), v(std::move(iv)) {}

  Quaternion operator+(const Quaternion& rhs) const;
  Quaternion& operator+=(const Quaternion& rhs) { return *this = *this + rhs; }
  Quaternion operator-(const Quaternion& rhs) const;
  Quaternion& operator-=(const Quaternion& rhs) { return *this = *this - rhs; }

  Quaternion operator*(const Quaternion& rhs) const;
  Quaternion& operator*=(const Quaternion& rhs) { return *this = *this * rhs; }
  Quaternion operator*(double r) const;
  Quaternion& operator*=(double r) { return *this = *this * r; }
  friend Quaternion operator*(double r, const Quaternion& rhs) {
    return rhs * r;
  }
  Quaternion operator/(double r) const;
  Quaternion& operator/=(double r) { return *this = *this / r; }

  Quaternion conj(void) const;

  double sqrnorm(void) const;
  double norm(void) const;

  double Dot(const Quaternion&) const;

  Quaternion inv(void) const;

 public:
  double s;
  Vector3 v;
};

class QuaternionTransform : public ITransformation {
 public:
  QuaternionTransform(Quaternion quat) : q(std::move(quat)), qinv(q.inv()) {}
  QuaternionTransform(Quaternion quat, Quaternion quatinv)
      : q(std::move(quat)), qinv(std::move(quatinv)) {}

  Point3 Doit(Point3) const override;
  Point3 Undo(Point3) const override;
  Vector3 Doit(Vector3) const override;
  Vector3 Undo(Vector3) const override;
  Normal Doit(Normal) const override;
  Normal Undo(Normal) const override;

 private:
  Quaternion q, qinv;

  basic_vector<double, 3> Doit_impl(const basic_vector<double, 3>&) const;
  basic_vector<double, 3> Undo_impl(const basic_vector<double, 3>&) const;

 public:
  /**
   * @brief creates a quaternion representing a rotation.
   *
   * @param A 3D vector representing the axis of rotation.
   * @param The angle of rotation in radians. The rotation is applied
   * counter-clockwise when looking along the direction of the axis vector.
   *
   * @return QuaternionTransform The resulting quaternion encapsulates the
   * rotation.
   *
   * Example usage:
   * auto qt = QuaternionTransform::Rotate(Vector3(0, 0, 1), M_PI / 4);
   * // Rotate 45 degrees around the Z-axis
   */
  static QuaternionTransform Rotate(const basic_vector<double, 3>&, double);
  static QuaternionTransform RotateFrTo(Vector3 fr, Vector3 to);

  /**
   * @brief Creates a quaternion transformation that aligns the global
   * coordinate system to a given local coordinate system.
   *
   * @param The basis of the local coordinate system.
   *
   * @return QuaternionTransform representing the rotation world->local.
   *
   * Note: the basis must form a proper coordinate system.
   */
  static QuaternionTransform AlignXYZ(const basic_vector<Vector3, 3>&);
  static QuaternionTransform AlignXYZ(Vector3, Vector3, Vector3);
};

class CompositeTransformation : public ITransformation {
 public:
  CompositeTransformation(
      std::initializer_list<std::shared_ptr<ITransformation>> sub)
      : subtr_(std::move(sub)) {}
  template <typename... Ts>
  CompositeTransformation(Ts&&... params)
      : subtr_({std::forward<Ts>(params)...}) {}

  Point3 Doit(Point3) const override;
  Point3 Undo(Point3) const override;
  Vector3 Doit(Vector3) const override;
  Vector3 Undo(Vector3) const override;
  Normal Doit(Normal) const override;
  Normal Undo(Normal) const override;

 private:
  std::vector<std::shared_ptr<ITransformation>> subtr_;
};
