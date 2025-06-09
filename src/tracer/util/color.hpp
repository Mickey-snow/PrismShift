#pragma once

#include "vecmath.hpp"

class Color : public basic_vector<double, 3> {
 private:
  using super = basic_vector;

 public:
  using super::super;
};

Color Format_Color(const Color& pixel_color, const double& scale = 255.0);
