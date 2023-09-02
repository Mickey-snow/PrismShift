#include<shapes/sphere.hpp>
#include<factory.hpp>
#include<util/util.hpp>
#include<bvh.hpp>

#include<gtest/gtest.h>
#include<sstream>

const std::string sphere_shapeid("sphere");

TEST(sphere, registerfactory){
  EXPECT_TRUE(ShapeFactory::Instance()->isRegistered(sphere_shapeid));
}


TEST(sphere, instantiateUsingFactory){
  std::stringstream ss("0 0 0 1");
  std::shared_ptr<Shape> ball = (ShapeFactory::Instance()->GetCreateFn(sphere_shapeid))(ss);

  EXPECT_EQ(ball->Get_Bounding_box(), AABB(Point3(1,1,1), Point3(-1,-1,-1)));
}
