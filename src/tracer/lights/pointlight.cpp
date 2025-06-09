#include "pointlight.hpp"

Pointlight::Pointlight() : color(0, 0, 0) {}

Pointlight& Pointlight::Set_Color(const Color& col) {
  color = col;
  return *this;
}

Pointlight& Pointlight::Set_Color(const double& r,
                                  const double& g,
                                  const double& b) {
  color = Color(r, g, b);
  return *this;
}
