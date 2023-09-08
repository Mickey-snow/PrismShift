#include<util/util.hpp>

#include<gtest/gtest.h>
#include "vec-comparer.hpp"

TEST(vec3, plus){
  Vector3 a(42,-12,4), b(0.4,321,8.9);
  Vector3 sum(42.4, 309, 12.9);
  EXPECT_TRUE(Vec3eq(a+b,sum));
}

TEST(vec3, crossproduct){
  Vector3 a(1,0,0),b(0,0,1);
  Vector3 cross_product = Vector3::Cross(a,b);
  EXPECT_TRUE(Vec3eq(cross_product, 0,-1,0));
}
