#pragma once

#include <gmock/gmock.h>

#include <shape.hpp>
#include <util/util.hpp>

class mShape : public IShape {
 public:
  mShape() {}

  MOCK_METHOD(HitRecord,
              Hit,
              (const Ray& r, const Interval<double>& period),
              (const, override));
  MOCK_METHOD(AABB, GetBbox, (), (const, override));
};
