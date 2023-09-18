#include<gtest/gtest.h>

#include<util/util.hpp>

TEST(coord, rotate){
  Coordinate3 frame;
  frame.Set_Rotation(Transformation::RotateY(pi/2));

  Vector3 v{1,0,0},vp{0,0,1};
  EXPECT_EQ(frame.Local2World(v), vp);
  EXPECT_EQ(frame.World2Local(vp), v);
}

TEST(coord, translateRotate){
  Coordinate3 frame;
  frame.Set_Translation(Transformation::Translate(1,1,1))
    .Set_Rotation(Transformation::RotateZ(pi));

  Vector3 v{1,0,0}, vp{-1,0,0};
  EXPECT_EQ(frame.Local2World(v), vp);
  EXPECT_EQ(frame.World2Local(vp), v);

  Point3 p{1,0,0}, pp{0,1,1};
  EXPECT_EQ(frame.Local2World(v), vp);
  EXPECT_EQ(frame.World2Local(vp), v);
}

TEST(coord, localCoord){
  auto i=Vector3::Random_Unit(), j=Vector3::Random_Unit(), k=Vector3::Random_Unit();
  Vector3 translation = Vector3::Random(-10,10);
  Point3 Origin = (Point3)translation;
  
  auto frame = Coordinate3()
    .Set_Rotation(Coordinate3::AlignXYZ(i,j,k))
    .Set_Translation(Coordinate3::Origin(Origin));
    
  auto a=random_double(-10,10), b=random_double(-10,10), c=random_double(-10,10);
  Point3 plocal{a,b,c};
  Point3 pworld = Point3{0,0,0} + translation + a*i + b*j + c*k;

  EXPECT_EQ(frame.Local2World(plocal), pworld);
  EXPECT_EQ(frame.World2Local(pworld), plocal);
}
