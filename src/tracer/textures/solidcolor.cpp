#include "solidcolor.hpp"

SolidColor::SolidColor() : color(0,0,0) {}

SolidColor& SolidColor::Set_Color(const double &r, const double &g, const double &b){
  color = Color(r,g,b);
  return *this;
}

SolidColor& SolidColor::Set_Color(const Color& col){
  color = col;
  return *this;
}

class Point2;
Color SolidColor::ColorAt(const Point2&) {
  return color;
}
