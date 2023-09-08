#include<util/util.hpp>

#include "vec-comparer.hpp"
#include "constant.hpp"
#include<gtest/gtest.h>


TEST(ray, intersection){
  auto Q=Point3(0,0,0), u=Vector3(0,1,0), v=Vector3(1,0,1);
  auto r = Ray(Point3(0,0,-10), Point3(2.5,5,2.5)-Point3(0,0,-10));

  double time = r.intersectionTimeWithPlane(Q,u,v);
  ASSERT_FALSE(std::isnan(time));
    
  Point3 intersection = r.At(time);  
  EXPECT_TRUE(Vec3eq(intersection, 2.5,5,2.5));
}
