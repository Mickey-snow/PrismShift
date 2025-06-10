#pragma once

#include <util/aabb.hpp>
#include <util/geometry.hpp>
#include <util/ray.hpp>
#include <util/transform.hpp>

#include <memory>
#include <optional>
#include <string>

class AABB;
class Ray;
struct HitRecord;
template <typename T>
class Interval;
class Primitive;
class IMaterial;

class IShape {
 public:
  virtual ~IShape() = default;

  virtual HitRecord Hit(const Ray&, const Interval<double>&) const = 0;
  virtual AABB GetBbox(void) const = 0;
};

class ConcreteShape : public IShape {
 public:
  ConcreteShape();
  ~ConcreteShape();

  virtual HitRecord Hit(const Ray&, const Interval<double>&) const override;
  virtual AABB GetBbox(void) const override;

  ConcreteShape& Set_Shape(IShape const* shape) {
    m_shape = shape;
    m_bbox_rec.reset();
    return *this;
  }
  ConcreteShape& Set_Frame(const MatrixTransformation& frame) {
    m_frame = frame;
    m_bbox_rec.reset();
    return *this;
  }

 private:
  IShape const* m_shape;
  MatrixTransformation m_frame;
  mutable std::optional<AABB> m_bbox_rec;
};

struct HitRecord {
  HitRecord() = default;

  bool hits = false;
  Primitive const* hitted_obj = nullptr;
  double time;
  Point3 position;

  Ray ray;
  Normal normal;  // normal is at the same side with ray
  bool front_face;

  IMaterial const* material = nullptr;

  void Set_Face_Normal(Ray r, Normal outward_normal) {
    front_face = Vector3::Dot(r.Direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
    ray = r;
  }

  static HitRecord RTN(Ray ray, double time, Normal normal) {
    HitRecord rec;
    rec.hits = true;
    rec.hitted_obj = nullptr;
    rec.time = time;
    rec.position = ray.At(time);
    rec.Set_Face_Normal(ray, normal);
    return rec;
  }
};
