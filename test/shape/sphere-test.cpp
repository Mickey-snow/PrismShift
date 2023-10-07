#include<gtest/gtest.h>

#include<shapes/3d/sphere.hpp>
#include<factory.hpp>
#include<util/util.hpp>

#include<sstream>
#include<string>

const std::string sphere_shapeid("sphere");

TEST(sphere, registerfactory){
  EXPECT_TRUE(ShapeFactory::Instance()->isRegistered(sphere_shapeid));
}


TEST(sphere, instantiateUsingFactory){
  std::vector<Attribute> attr{
    {"position", Point3(1,1,1)},
    {"radius", 1.0}
  };

  std::shared_ptr<Shape> ball = (ShapeFactory::Instance()->GetCreateFn(sphere_shapeid))(attr);

  EXPECT_EQ(ball->Get_Bounding_box(), AABB(Point3(2,2,2), Point3(0,0,0)));
}
