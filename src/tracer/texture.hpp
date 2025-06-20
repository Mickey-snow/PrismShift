#pragma once

#include "util/util.hpp"

#include <memory>

template <typename T>
class ITexture {
 public:
  virtual ~ITexture() = default;
  virtual T Evaluate(Point2 uv) const = 0;
};
template <typename T>
using Texture = std::shared_ptr<ITexture<T>>;

class SolidColor : public ITexture<Color> {
  Color col_;

 public:
  SolidColor(double r, double g, double b) : SolidColor(Color(r, g, b)) {}
  SolidColor(Color col = {}) : col_(col) {}

  Color Evaluate(Point2) const override { return col_; }
};

class FloatConst : public ITexture<double> {
  double val_;

 public:
  constexpr FloatConst(double v) : val_(v) {}

  double Evaluate(Point2) const override { return val_; }
};

class DebugTexture : public ITexture<Color> {
 public:
  DebugTexture() = default;

  Color Evaluate(Point2 uv) const override {
    if (uv.x() < 0.5)
      return uv.y() < 0.5 ? Color(1, 0, 0) : Color(0, 1, 0);
    else
      return uv.y() < 0.5 ? Color(0, 0, 1) : Color(1, 1, 1);
  }
};

template <typename T>
inline auto make_texture(T param) {
  if constexpr (std::same_as<T, Color>)
    return std::make_shared<SolidColor>(std::move(param));

  else if constexpr (arithmetic<T>)
    return std::make_shared<FloatConst>(param);

  else
    static_assert(always_false_v<T>);
}
