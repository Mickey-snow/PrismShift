#include<geometry.hpp>

#include<gtest/gtest.h>
#include<cmath>


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



class Vector2Test : public ::testing::Test{
protected:
  void SetUp() override{
    v1 = Vector2(5,-1);
    v2 = Vector2(0.3, -1.2);
  }

  Vector2 orig;
  Vector2 v1,v2;
};

constexpr double EPS = 1e-8;

TEST_F(Vector2Test, getxycomponentMethodvsOperator){
  EXPECT_NEAR(v1.x(), v1[0], EPS);
  EXPECT_NEAR(v1.y(), v1[1], EPS);
  EXPECT_NEAR(v2.x(), v2[0], EPS);
  EXPECT_NEAR(v2.y(), v2[1], EPS);
}
  
TEST_F(Vector2Test, init){
  EXPECT_FALSE(Vec2eq(orig, 0,1));
  EXPECT_TRUE(Vec2eq(orig,0,0));
  EXPECT_TRUE(Vec2eq(v1,5,-1));
  EXPECT_TRUE(Vec2eq(v2,0.3,-1.2));
}

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
