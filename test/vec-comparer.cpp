#include "vec-comparer.hpp"
#include<gtest/gtest.h>

#include<util/util.hpp>

#include<iostream>
#include<cmath>
#include<format>

testing::AssertionResult Vec3eq(Vector3 vec, double x, double y,double z){
  static const double EPS = 1e-4;
  if(fabs(vec[0]-x)+fabs(vec[1]-y)+fabs(vec[2]-z) < EPS)
    return testing::AssertionSuccess() << "OK "<<vec<<std::format(" is near ({},{},{})", x,y,z);
  else return testing::AssertionFailure() << std::format("Expect near: ({},{},{})",x,y,z)<<" Actual: "<<vec;
}
testing::AssertionResult Vec3eq(Vector3 vec1, Vector3 vec2){
  return Vec3eq(vec1, vec2.x(), vec2.y(), vec2.z());
}

testing::AssertionResult Vec2eq(Vector2 vec, double x,double y){
  static const double EPS = 1e-4;
  if(fabs(vec[0]-x)+fabs(vec[1]-y) < EPS)
    return testing::AssertionSuccess() << "OK "<<vec<< std::format(" is near ({},{})", x,y);
  else return testing::AssertionFailure() << "Expect: "<<std::format("({},{})", x,y)<<" Actual: "<<vec;
}
testing::AssertionResult Vec2eq(Vector2 vec1, Vector2 vec2){
  return Vec2eq(vec1, vec2.x(), vec2.y());
}
