#include<iostream>
#include "vector2.hpp"

std::ostream& operator << (std::ostream& out, const Vector2& v){
  out<<'('<<v[0]<<','<<v[1]<<')';
  return out;
}
