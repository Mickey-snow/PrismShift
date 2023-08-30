#ifndef COLOR_H
#define COLOR_H

#include<iostream>
#include "vector3.hpp"

using Color = Vector3;

Color Format_Color(const Color& pixel_color);

void Write_Color(std::ostream& out, const Color& pixel_color);

#endif
