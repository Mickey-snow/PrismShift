#pragma once

#include "util/ray.hpp"
#include "util/vector.hpp"

class Primitive;
struct Material;

struct HitRecord {
  HitRecord() = default;

  bool hits = false;
  Primitive const* primitive = nullptr;
  double time;
  Point3 position;

  Ray ray;
  Normal normal;  // normal is at the same side with ray
  bool front_face;

  Material const* material = nullptr;

  void SetFaceNormal(const Ray& r, Normal outward_normal) {
    front_face = Vector3::Dot(r.Direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
    ray = r;
  }

  static HitRecord RTN(Ray ray, double time, Normal normal) {
    HitRecord rec;
    rec.hits = true;
    rec.primitive = nullptr;
    rec.time = time;
    rec.position = ray.At(time);
    rec.SetFaceNormal(ray, normal);
    return rec;
  }
};
