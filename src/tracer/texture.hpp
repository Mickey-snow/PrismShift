#pragma once

#include "util/util.hpp"

template <typename T>
class Texture {
 public:
  virtual ~Texture() = default;
  virtual T Evaluate(Point2 uv) = 0;
};

class SolidColor : public Texture<Color> {
  Color col_;

 public:
  SolidColor(Color col = {}) : col_(col) {}

  Color Evaluate(Point2) override { return col_; }
};
