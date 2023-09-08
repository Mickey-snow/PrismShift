#include<shapes/2d/triangle.hpp>
#include<shapes/2d/plane.hpp>
#include<util/util.hpp>
#include<shape.hpp>
#include<scene.hpp>

#include<gtest/gtest.h>
#include "vec-comparer.hpp"
#include "constant.hpp"


TEST(triangle, rayintersection){
  Vector3 from,to;
  auto tri = Triangle::From_points(Point3(50,0,0), Point3(50,0,40), Point3(0,0,40));

  from = Vector3(70,10,50); to = Vector3(45,0,37);
  Ray r = Ray(from,to-from);
  auto rec = tri->Ray_Hit(r, Interval<double>(0.9,1.1));
  ASSERT_TRUE(rec.hits);
  EXPECT_TRUE(Vec3eq(rec.position, to));

  to = Vector3(40,0,4);
  r = Ray(from,to-from);
  rec = tri->Ray_Hit(r, Interval<double>::Positive());
  ASSERT_FALSE(rec.hits);
}

TEST(triangleScene, rayintersection){
  auto bottom1 = std::make_shared<Triangle>(*Triangle::From_points(Point3(0,0,40), Point3(0,0,0), Point3(50,0,0))),
    bottom2 = std::make_shared<Triangle>(*Triangle::From_points(Point3(50,0,0), Point3(0,0,40), Point3(50,0,40)));
  auto left = std::make_shared<Triangle>(*Triangle::From_points(Point3(0,0,0), Point3(50,0,0), Point3(0,10,0)));
  auto front = std::make_shared<Triangle>(*Triangle::From_points(Point3(0,0,0), Point3(0,10,0), Point3(0,10,40)));
  Scene world(std::vector<std::shared_ptr<Visible>>{
      bottom1,bottom2,left,front});
  world.Build_BVH();

  Vector3 to,fr;
  fr = Point3(100,100,100); to = Point3(25,4.78,0);
  Ray r = Ray(fr,to-fr);
  auto rec = world.Ray_Hit(r, Interval<double>(0.9,1.1));
  ASSERT_TRUE(rec.hits); EXPECT_TRUE(Vec3eq(rec.position, to));
  EXPECT_STREQ(rec.hitted_obj->Get_Name().c_str(), "triangle");
  EXPECT_NEAR(rec.time, 1, EPS);

  to = Point3(0,9.9,20);
  r = Ray(fr, to-fr);
  rec = world.Ray_Hit(r, Interval<double>(0.9,1.1));
  ASSERT_TRUE(rec.hits); EXPECT_TRUE(Vec3eq(rec.position, to));
  EXPECT_STREQ(rec.hitted_obj->Get_Name().c_str(), "triangle");
    EXPECT_NEAR(rec.time, 1, EPS);

  to = Point3(25,0,20);
  r = Ray(fr,to-fr);
  rec = world.Ray_Hit(r, Interval<double>(0.9,1.1));
  EXPECT_TRUE(rec.hits);
  EXPECT_NEAR(rec.time, 1, EPS);

  world.Add(std::make_shared<Plane>(Vector3(0,-5.001,0),Vector3(1,0,0),Vector3(0,0,1)));
  to = Point3(10,0,10);
  r = Ray(fr,to-fr);
  rec = world.Ray_Hit(r, Interval<double>(0.9,1.1));
  ASSERT_TRUE(rec.hits); EXPECT_TRUE(Vec3eq(rec.position,to));
  EXPECT_STREQ(rec.hitted_obj->Get_Name().c_str(), "triangle");
    EXPECT_NEAR(rec.time, 1, EPS);
}
