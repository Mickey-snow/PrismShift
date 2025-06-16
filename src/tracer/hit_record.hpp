#pragma once

#include "util/ray.hpp"
#include "util/vector.hpp"

class Primitive;
struct Material;

struct HitRecord {
  HitRecord() = default;

  // note: everythin in local coordinate system
  bool hits = false;
  Primitive const* primitive = nullptr;
  double time;
  Point3 position;
  Normal normal;

  static HitRecord RTN(Ray ray, double time, Normal normal) {
    HitRecord rec;
    rec.hits = true;
    rec.primitive = nullptr;
    rec.time = time;
    rec.position = ray.At(time);
    rec.normal = std::move(normal);
    return rec;
  }
};
