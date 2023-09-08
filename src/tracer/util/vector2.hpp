#ifndef VEC2_H
#define VEC2_H

#include<iostream>

class Vector2{
public:
  Vector2():e{0,0} {}
  Vector2(const double& ix,const double& iy) : e{ix,iy} {}

  double x() const{ return e[0]; }
  double y() const{ return e[1]; }

  double operator [] (const int& i) const{ return e[i]; }
  double& operator [] (const int& i){ return e[i]; }
  
private:
  double e[2];
};
using Point2 = Vector2;

std::ostream& operator << (std::ostream& out, const Vector2& v);


#endif
