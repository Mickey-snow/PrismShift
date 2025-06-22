#pragma once

#include "util/ray.hpp"
#include "util/vector.hpp"

class Primitive;
struct IMaterial;

struct HitRecord {
  // note: everythin in local coordinate system
  bool hits = false;
  Primitive const* primitive = nullptr;
  Float time;
  Point3 position;
  Point2 uv;
  Normal normal;  // outward normal

  static constexpr HitRecord Create(Float time,
                                    Point3 pos,
                                    Point2 uv,
                                    Normal n) {
    HitRecord rec;
    rec.hits = true;
    rec.time = time;
    rec.position = pos;
    rec.uv = uv;
    rec.normal = n;
    return rec;
  }
};
