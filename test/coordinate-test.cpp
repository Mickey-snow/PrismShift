#include<gtest/gtest.h>
#include<util/coordinate.hpp>
#include<util/geometry.hpp>
#include<util/transform.hpp>

constexpr double pi = std::numbers::pi;

TEST(coord, rotate){
  Coordinate3 frame;
  frame.Set_Rotate(Transformation::RotateY(pi/2));

  Vector3 v{1,0,0},vp{0,0,1};
  EXPECT_EQ(frame.Local2World(v), vp);
  EXPECT_EQ(frame.World2Local(vp), v);
}

TEST(coord, translateRotate){
  Coordinate3 frame;
  frame.Set_Translate(Transformation::Translate(1,1,1))
    .Set_Rotate(Transformation::RotateZ(pi));

  Vector3 v{1,0,0}, vp{-1,0,0};
  EXPECT_EQ(frame.Local2World(v), vp);
  EXPECT_EQ(frame.World2Local(vp), v);

  Point3 p{1,0,0}, pp{0,1,1};
  EXPECT_EQ(frame.Local2World(v), vp);
  EXPECT_EQ(frame.World2Local(vp), v);
}
