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

  void SetFaceNormal(Ray r, Normal outward_normal) {
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
    rec.SetFaceNormal(ray, normal);
    return rec;
  }
};
