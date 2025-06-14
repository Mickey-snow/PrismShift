#pragma once

#include "util/color.hpp"
#include "util/ray.hpp"

class ILight {
 public:
  virtual ~ILight() = default;

  virtual Color Le(const Ray& r) const;
};

class Light : public ILight {
  Color col_;

 public:
  explicit Light(Color c) : col_(std::move(c)) {}
  virtual Color Le(const Ray& r) const override { return col_; }
};
