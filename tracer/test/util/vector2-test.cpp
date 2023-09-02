#include<util/vector2.hpp>

#include<gtest/gtest.h>
#include<cmath>
#include<format>

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

testing::AssertionResult Vec2eq(Vector2 vec, double x,double y){
  if(fabs(vec[0]-x+vec[1]-y) < EPS)
    return testing::AssertionSuccess() << "OK "<<vec<< std::format(" is near ({},{})", x,y);
  else return testing::AssertionFailure() << "Expect: "<<std::format("({},{})", x,y)<<" Actural: "<<vec;
}
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
