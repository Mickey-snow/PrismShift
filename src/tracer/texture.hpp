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
  SolidColor(double r, double g, double b) : SolidColor(Color(r, g, b)) {}
  SolidColor(Color col = {}) : col_(col) {}

  Color Evaluate(Point2) override { return col_; }
};

class FloatConst : public Texture<double> {
  double val_;

 public:
  constexpr FloatConst(double v) : val_(v) {}

  double Evaluate(Point2) override { return val_; }
};
