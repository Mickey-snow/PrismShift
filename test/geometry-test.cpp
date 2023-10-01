#include<util/geometry.hpp>

#include<gtest/gtest.h>
#include<gmock/gmock.h>

#include<cmath>
#include<ranges>
#include<string>


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

    CheckSetUp();
  }

  Vector2 orig;
  Vector2 v1,v2;
  Vector2 v3{10,10};
  double sc = 1.5;

  static constexpr double EPS = 1e-8;

  void CheckSetUp() const{
    ASSERT_FALSE(Vec2eq(orig, 0,1));
    ASSERT_TRUE(Vec2eq(orig,0,0));
    ASSERT_TRUE(Vec2eq(v1,5,-1));
    ASSERT_TRUE(Vec2eq(v2,0.3,-1.2));
    ASSERT_TRUE(Vec2eq(v3, 10, 10));
  }
};


TEST_F(Vector2Test, getxycomponentMethodSameAsIndexOperator){
  EXPECT_NEAR(v1.x(), v1[0], EPS);
  EXPECT_NEAR(v1.y(), v1[1], EPS);
  EXPECT_NEAR(v2.x(), v2[0], EPS);
  EXPECT_NEAR(v2.y(), v2[1], EPS);
}
  
TEST_F(Vector2Test, plus){
  EXPECT_TRUE(Vec2eq(v1+v2, 5.3, -2.2));
  EXPECT_TRUE(Vec2eq(v2+v3, 10.3, 8.8));
}

TEST_F(Vector2Test, minus){
  EXPECT_TRUE(Vec2eq(v1-v2, 4.7, 0.2));
}

TEST_F(Vector2Test, timesScalar){
  EXPECT_TRUE(Vec2eq(v1*sc, 7.5,-1.5));
  EXPECT_TRUE(Vec2eq(sc*v1, 7.5,-1.5));
}

TEST_F(Vector2Test, timesZero){
  const double zero = 0;
  for(const auto& v : { orig, v1,v2,v3}){
    EXPECT_TRUE((v*zero).Near_Zero());
    EXPECT_TRUE((zero*v).Near_Zero());
  }
}

TEST_F(Vector2Test, dividebyScalar){
  EXPECT_TRUE(Vec2eq(v2/sc, 0.2, -0.8));
}

TEST_F(Vector2Test, vec2AcceptsNearAsEqual){
  double delta = 1e-4;
  std::vector<Vector2> v_arr{orig,v1,v2,v3};
  
  for(const auto& v : v_arr){
    auto vp = v + Vector2{delta,-delta};
    EXPECT_EQ(v,vp);
  }

  delta = 1e-1;
  for(const auto& v : v_arr){
    auto vp = v + Vector2{-delta, delta};
    EXPECT_NE(v,vp);
  }
}

TEST_F(Vector2Test, dotProduct){
  EXPECT_DOUBLE_EQ(v1.Dot(v2), 2.7);
  EXPECT_DOUBLE_EQ(v3.Dot(orig), 0);
}

TEST_F(Vector2Test, length){
  EXPECT_DOUBLE_EQ(v1.Length_squared(), 26);
  EXPECT_DOUBLE_EQ(v2.Length_squared(), 1.53);
  EXPECT_DOUBLE_EQ(v3.Length_squared(), 200);
}


class Vector3Test : public ::testing::Test{
protected:
  void SetUp(){
    v1 = Vector3{42,-12,4};
    v2 = Vector3{0.4,321,8.9};
  }

  Vector3 v1,v2;
  Vector3 i{1,0,0},j{0,1,0},k{0,0,1};
  Vector3 orig;
};

TEST_F(Vector3Test, plus){
  EXPECT_TRUE(Vec3eq(v1+v2, 42.4, 309, 12.9));
}

TEST_F(Vector3Test, crossproduct){
  EXPECT_TRUE(Vec3eq(Vector3::Cross(i,j), 0,0,1));
  EXPECT_TRUE(Vec3eq(Vector3::Cross(v1,v2), -1390.8, -372.2, 13486.8));
}


TEST(VecBaseTest, explicitConvert){
  Vector3 a{50,15,21}; Point3 b{32,24,31};
  auto cross_product = Vector3::Cross(a,(Vector3)b);
  EXPECT_TRUE(Vec3eq(cross_product, -39,-878,720));
}

TEST(VecBaseTest, streamOutput){
  auto v = Vector3::Random_Unit();
  std::stringstream ss; ss << v;
  auto v_str = ss.str();
  EXPECT_THAT(v_str, testing::MatchesRegex("[\[(].*,.*,.*[\])]"));
}
