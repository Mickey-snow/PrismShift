#include "transform.hpp"
#include "geometry.hpp"

#include <cassert>
#include <cmath>

VectorTranslate::VectorTranslate(const Vector3& p)
    : dx(p.x()), dy(p.y()), dz(p.z()) {}

Point3 VectorTranslate::Doit(const Point3& p) const {
  return Point3{p.x() + dx, p.y() + dy, p.z() + dz};
}
Point3 VectorTranslate::Undo(const Point3& p) const {
  return Point3{p.x() - dx, p.y() - dy, p.z() - dz};
}
Vector3 VectorTranslate::Doit(const Vector3& v) const { return v; }
Vector3 VectorTranslate::Undo(const Vector3& v) const { return v; }
Normal VectorTranslate::Doit(const Normal& n) const { return n; }
Normal VectorTranslate::Undo(const Normal& n) const { return n; }

VectorScale::VectorScale(const Vector3& v) : dx(v.x()), dy(v.y()), dz(v.z()) {}

Point3 VectorScale::Doit(const Point3& p) const {
  return Point3(p.x() * dx, p.y() * dy, p.z() * dz);
}
Point3 VectorScale::Undo(const Point3& p) const {
  return Point3(p.x() / dx, p.y() / dy, p.z() / dz);
}
Vector3 VectorScale::Doit(const Vector3& v) const {
  return Vector3(v.x() * dx, v.y() * dy, v.z() * dz);
}
Vector3 VectorScale::Undo(const Vector3& v) const {
  return Vector3(v.x() / dx, v.y() / dy, v.z() / dz);
}
/**
 * @brief Scales a normal inversely along the x, y, and z axes.
 *
 * Applies the scaling transformation to a `Normal` object.
 * Unlike points and vectors, each component of the
 * normal is divided by its corresponding scaling factor.
 * This is done to ensure that the transformed normal
 * remains perpendicular to the surface after scaling.
 *
 * @return Normal The transformed normal.
 */
Normal VectorScale::Doit(const Normal& n) const {
  return Normal(n.x() / dx, n.y() / dy, n.z() / dz);
}
Normal VectorScale::Undo(const Normal& n) const {
  return Normal(n.x() * dx, n.y() * dy, n.z() * dz);
}

Point3 MatrixTransformation::Doit(const Point3& p) const {
  basic_vector<double, 4> w(p.x(), p.y(), p.z(), 1);
  w = m * w;
  static constexpr auto EPS = 1e-9;
  auto ans = Point3(w.x(), w.y(), w.z());
  if (fabs(w[3] - 1.0) > EPS)
    ans /= w[3];
  return ans;
}
Point3 MatrixTransformation::operator()(const Point3& p) const {
  return Doit(p);
}

Vector3 MatrixTransformation::Doit(const Vector3& p) const {
  basic_vector<double, 4> w(p.x(), p.y(), p.z(), 0);
  w = m * w;
  return Vector3(w.x(), w.y(), w.z());
}
Vector3 MatrixTransformation::operator()(const Vector3& p) const {
  return Doit(p);
}

/**
 * @brief Transforms a normal using a 4x4 homogeneous transformation matrix.
 *
 * This method applies a transformation to a normal,
 * ensuring that the transformed normal remains perpendicular to
 * the tangent plane of the surface.
 * Unlike transformations applied to points or vectors, normals require
 * a special handling to maintain their geometric properties after the
 * transformation. The transformation for normals is not done directly using the
 * original matrix M, but instead using the matrix S, which is derived from the
 * transpose of the inverse of M (S = (M^-1)^T).
 *
 * @return A new Normal object: the transformed normal.
 */
Normal MatrixTransformation::Doit(const Normal& p) const {
  basic_vector<double, 4> w(p.x(), p.y(), p.z(), 0);
  w = minv.T() * w;
  return Normal(w.x(), w.y(), w.z());
}

Normal MatrixTransformation::operator()(const Normal& p) const {
  return Doit(p);
}

Point3 MatrixTransformation::Undo(const Point3& p) const {
  return Inverse().Doit(p);
}
Vector3 MatrixTransformation::Undo(const Vector3& p) const {
  return Inverse().Doit(p);
}
Normal MatrixTransformation::Undo(const Normal& p) const {
  return Inverse().Doit(p);
}

MatrixTransformation MatrixTransformation::Translate(const Vector3& p) {
  return MatrixTransformation(
      Matrix4{1, 0, 0, p.x(), 0, 1, 0, p.y(), 0, 0, 1, p.z(), 0, 0, 0, 1},
      Matrix4{1, 0, 0, -p.x(), 0, 1, 0, -p.y(), 0, 0, 1, -p.z(), 0, 0, 0, 1});
}
MatrixTransformation MatrixTransformation::Translate(const double& dx,
                                                     const double& dy,
                                                     const double& dz) {
  return Translate(Vector3{dx, dy, dz});
}

MatrixTransformation MatrixTransformation::Rotate(basic_vector<double, 3> axis,
                                                  const double& costheta,
                                                  const double& sintheta) {
  axis = axis.Normalized();

  auto R = [](const basic_vector<double, 3>& axis, const double& costheta,
              const double& sintheta) {
    const double nx = axis.x(), ny = axis.y(), nz = axis.z();
    return Matrix4{nx * nx * (1 - costheta) + costheta,
                   nx * ny * (1 - costheta) + nz * sintheta,
                   nx * nz * (1 - costheta) - ny * sintheta,
                   0,
                   nx * ny * (1 - costheta) - nz * sintheta,
                   ny * ny * (1 - costheta) + costheta,
                   ny * nz * (1 - costheta) + nx * sintheta,
                   0,
                   nx * nz * (1 - costheta) + ny * sintheta,
                   ny * nz * (1 - costheta) - nx * sintheta,
                   nz * nz * (1 - costheta) + costheta,
                   0,
                   0,
                   0,
                   0,
                   1};
  };

  auto RotateMat = R(axis, costheta, sintheta);
  auto RotateMatInv = R(axis, costheta, -sintheta);

  return MatrixTransformation(RotateMat, RotateMatInv);
}
MatrixTransformation MatrixTransformation::Rotate(
    const basic_vector<double, 3>& axis,
    const double& theta) {
  double costheta = cos(theta);
  double sintheta = sin(theta);
  return Rotate(axis, costheta, sintheta);
}
MatrixTransformation MatrixTransformation::RotateX(const double& theta) {
  return Rotate(Vector3{1, 0, 0}, theta);
}
MatrixTransformation MatrixTransformation::RotateX(const double& costheta,
                                                   const double& sintheta) {
  return Rotate(Vector3{1, 0, 0}, costheta, sintheta);
}
MatrixTransformation MatrixTransformation::RotateY(const double& theta) {
  return Rotate(Vector3{0, 1, 0}, theta);
}
MatrixTransformation MatrixTransformation::RotateY(const double& costheta,
                                                   const double& sintheta) {
  return Rotate(Vector3{0, 1, 0}, costheta, sintheta);
}
MatrixTransformation MatrixTransformation::RotateZ(const double& theta) {
  return Rotate(Vector3{0, 0, 1}, theta);
}
MatrixTransformation MatrixTransformation::RotateZ(const double& costheta,
                                                   const double& sintheta) {
  return Rotate(Vector3{0, 0, 1}, costheta, sintheta);
}

/**
 * @brief Rotates a unit vector `fr` to align it with another unit vector `to`.
 *
 * This method implements the rotation by calculating a rotation matrix derived
 * from a pair of reflection transformations. The algorithm first reflects the
 * `fr` vector to an intermediate vector `r`, and then reflects `r` to the `to`
 * vector. The product of these two reflections yields the desired rotation
 * matrix.
 *
 * The reflections are computed using the Householder matrix H(v), which
 * reflects a given vector `v` to its negation `-v`, while leaving all vectors
 * orthogonal to `v` unchanged. The Householder matrix is defined as: H(v) = I -
 * 2/(v.dot(v)) * vv^T, where I is the identity matrix.
 *
 * The rotation matrix `R` is then the product of two Householder matrices:
 * R = H(r-t) * H(r-f), where `r` is a vector orthogonal to both `fr` and `to`.
 *
 * @param `fr` A 3D unit vector representing the original direction.
 * @param `to` A 3D unit vector representing the target direction.
 * @return MatrixTransformation object representing the rotation.
 *
 * @note Requires `fr` and `to` to be unit vectors.
 */
MatrixTransformation MatrixTransformation::RotateFrTo(const Vector3& fr,
                                                      const Vector3& to) {
  assert(fr.Normalized() == fr);
  assert(to.Normalized() == to);
  Vector3 refl;
  if (std::abs(fr.x()) < 0.72 && std::abs(to.x()) < 0.72)
    refl = Vector3{1, 0, 0};
  else if (std::abs(fr.y()) < 0.72 && std::abs(to.y()) < 0.72)
    refl = Vector3{0, 1, 0};
  else
    refl = Vector3{0, 0, 1};

  Vector3 u = refl - fr, v = refl - to;
  Matrix4 r = Matrix4::I();
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      r[i][j] = ((i == j) ? 1 : 0) - 2 / u.Dot(u) * u[i] * u[j] -
                2 / v.Dot(v) * v[i] * v[j] +
                4 * u.Dot(v) / (u.Dot(u) * v.Dot(v)) * v[i] * u[j];
  return MatrixTransformation(r, r.T());
}

MatrixTransformation MatrixTransformation::AlignXYZ(const Vector3& i,
                                                    const Vector3& j,
                                                    const Vector3& k) {
  Matrix4 mat_rotate_toworld{i.x(), j.x(), k.x(), 0, i.y(), j.y(), k.y(), 0,
                             i.z(), j.z(), k.z(), 0, 0,     0,     0,     1};
  return MatrixTransformation(mat_rotate_toworld.inv(), mat_rotate_toworld);
};
MatrixTransformation MatrixTransformation::AlignXYZ(
    const basic_vector<Vector3, 3>& basis) {
  return MatrixTransformation::AlignXYZ(basis.x(), basis.y(), basis.z());
}

MatrixTransformation MatrixTransformation::Scale(const double& x,
                                                 const double& y,
                                                 const double& z) {
  return MatrixTransformation::Scale(Vector3{x, y, z});
}
MatrixTransformation MatrixTransformation::Scale(const Vector3& n) {
  auto ScaleMat = [](const double& kx, const double& ky, const double& kz) {
    return Matrix4{kx, 0, 0, 0, 0, ky, 0, 0, 0, 0, kz, 0, 0, 0, 0, 1};
  };

  auto mat = ScaleMat(n.x(), n.y(), n.z());
  auto matinv = ScaleMat(1.0 / n.x(), 1.0 / n.y(), 1.0 / n.z());

  return MatrixTransformation(mat, matinv);
}
MatrixTransformation MatrixTransformation::Scale(const Vector3& n,
                                                 const double& k) {
  auto ScaleMat = [](const Vector3& n, double k) {
    const double nx = n.x(), ny = n.y(), nz = n.z();
    --k;
    return Matrix4{1 + k * nx * nx,
                   k * nx * ny,
                   k * nx * nz,
                   0,
                   k * nx * ny,
                   1 + k * ny * ny,
                   k * ny * nz,
                   0,
                   k * nx * nz,
                   k * ny * nz,
                   1 + k * nz * nz,
                   0,
                   0,
                   0,
                   0,
                   1};
  };

  auto mat = ScaleMat(n, k);
  auto matinv = ScaleMat(n, 1.0 / k);

  return MatrixTransformation(mat, matinv);
}

Quaternion Quaternion::operator+(const Quaternion& rhs) const {
  return Quaternion(s + rhs.s, v + rhs.v);
}

Quaternion Quaternion::operator-(const Quaternion& rhs) const {
  return Quaternion(s - rhs.s, v - rhs.v);
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const {
  return Quaternion(s * rhs.s - v.Dot(rhs.v),
                    v.Cross(rhs.v) + s * rhs.v + rhs.s * v);
}

Quaternion Quaternion::operator*(const double& r) const {
  return Quaternion(r * s, r * v);
}

Quaternion Quaternion::operator/(const double& r) const {
  return Quaternion(s / r, v / r);
}

Quaternion Quaternion::conj(void) const { return Quaternion(s, -v); }

double Quaternion::sqrnorm(void) const { return this->Dot(*this); }
double Quaternion::norm(void) const { return sqrt(this->Dot(*this)); }

double Quaternion::Dot(const Quaternion& rhs) const {
  return s * rhs.s + v.Dot(rhs.v);
}

Quaternion Quaternion::inv(void) const { return conj() / sqrnorm(); }

basic_vector<double, 3> QuaternionTransform::Doit_impl(
    const basic_vector<double, 3>& it) const {
  auto p = Quaternion(0, it.x(), it.y(), it.z());
  p = q * p * qinv;
  return (basic_vector<double, 3>)p.v;
}
basic_vector<double, 3> QuaternionTransform::Undo_impl(
    const basic_vector<double, 3>& it) const {
  auto p = Quaternion(0, it.x(), it.y(), it.z());
  p = qinv * p * q;
  return (basic_vector<double, 3>)p.v;
}
Point3 QuaternionTransform::Doit(const Point3& pt) const {
  return (Point3)Doit_impl(pt);
}
Point3 QuaternionTransform::Undo(const Point3& pt) const {
  return (Point3)Undo_impl(pt);
}
Vector3 QuaternionTransform::Doit(const Vector3& v) const {
  return (Vector3)Doit_impl(v);
}
Vector3 QuaternionTransform::Undo(const Vector3& v) const {
  return (Vector3)Undo_impl(v);
}
Normal QuaternionTransform::Doit(const Normal& n) const {
  return (Normal)Doit_impl(n);
}
Normal QuaternionTransform::Undo(const Normal& n) const {
  return (Normal)Undo_impl(n);
}

// requires normalized n
QuaternionTransform QuaternionTransform::Rotate(
    const basic_vector<double, 3>& n,
    double theta) {
  assert(n.Normalized() == n);
  theta *= -0.5;
  auto q = Quaternion(cos(theta), sin(theta) * n);
  return QuaternionTransform(q);
}

QuaternionTransform QuaternionTransform::RotateFrTo(const Vector3& fr,
                                                    const Vector3& to) {
  double theta = acos(fr.Dot(to));
  auto axis = fr.Cross(to).Normalized();
  return QuaternionTransform::Rotate(axis, -theta);
}

// From the provided 4x4 matrix MatrixTransform::AlignXYZ,
// the upper-left 3x3 portion is the rotation matrix R,
// from which the corresponding quaternion can be derived as follows:
// w = 1/2 * sqrt(1+R11+R22+R33)
// x = (R32-R23) / 4w
// y = (R13-R31) / 4w
// z = (R21-R12) / 4w
// The quaternion should be normalized to ensure it represents a valid rotation.
QuaternionTransform QuaternionTransform::AlignXYZ(const Vector3& i,
                                                  const Vector3& j,
                                                  const Vector3& k) {
  double w = sqrt(1 + i.x() + j.y() + k.z()) / 2;
  double wquarter = 1.0 / (4 * w);
  double x = (j.z() - k.y()) * wquarter;
  double y = (k.x() - i.z()) * wquarter;
  double z = (i.y() - j.x()) * wquarter;

  auto q = Quaternion(w, x, y, z);
  q /= q.norm();
  return QuaternionTransform(q.inv(), q);
}
QuaternionTransform QuaternionTransform::AlignXYZ(
    const basic_vector<Vector3, 3>& basis) {
  return QuaternionTransform::AlignXYZ(basis.x(), basis.y(), basis.z());
}
