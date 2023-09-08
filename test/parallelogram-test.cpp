#include<shapes/2d/parallelogram.hpp>
#include<material.hpp>
#include<util/util.hpp>
#include<scene.hpp>

#include<memory>

#include<gtest/gtest.h>
#include "vec-comparer.hpp"

TEST(parallelogram, RayHit){
  auto quad = Parallelogram(Point3(0,0,0), Vector3(1,0,0), Vector3(0,1,0));

  Ray r = Ray(Point3(0,0,-1), Vector3(0.5,0.5,0));
  auto rec = quad.Ray_Hit(r, Interval<double>::Positive());
  EXPECT_FALSE(rec.hits);

  r = Ray(Point3(0,0,-2), Vector3(0.5,0.5,2));
  rec = quad.Ray_Hit(r, Interval<double>::Positive());
  EXPECT_TRUE(rec.hits);
  EXPECT_TRUE(Vec3eq(rec.position, 0.5,0.5,0));
}

TEST(parallelogram, RayHitinScene){
  std::shared_ptr<Visible> quad = std::make_shared<Parallelogram>(Point3(0,0,0), Vector3(0,10,0), Vector3(0,0,10));
  Scene world; world.Add(quad);

  auto Orig = Point3(20,0,0);
  Ray r = Ray(Orig , Point3(0,9.9,0)-Orig);
  auto rec = world.Ray_Hit(r, Interval<double>::Positive());
  ASSERT_TRUE(rec.hits);
  EXPECT_TRUE(Vec3eq(rec.position, 0,9.9,0));

  r = Ray(Orig, Point3(0,0.1,0.1)-Orig);
  rec = world.Ray_Hit(r, Interval<double>::Positive());
  ASSERT_TRUE(rec.hits);
  EXPECT_TRUE(Vec3eq(rec.position, 0,0.1,0.1));
}

TEST(parallelogram, frontface){
  auto quad = Parallelogram(Point3(0,0,0), Vector3(100,0,0), Vector3(0,0,100));

  Ray r = Ray(Point3(10,10,10), Vector3(0,-1,0));
  Hit_record rec = quad.Ray_Hit(r, Interval<double>::Positive());
  ASSERT_TRUE(rec.hits);
  EXPECT_TRUE(Vec3eq(rec.position, 10,0,10));
  EXPECT_TRUE(Vec3eq(rec.normal, 0,1,0));
  EXPECT_FALSE(rec.front_face);

  r = Ray(Point3(10,-10,10), Vector3(0,1,0));
  rec = quad.Ray_Hit(r, Interval<double>::Positive());
  ASSERT_TRUE(rec.hits);
  EXPECT_TRUE(Vec3eq(rec.normal, 0,-1,0));
  EXPECT_TRUE(rec.front_face);

}
