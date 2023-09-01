#include<gtest/gtest.h>
#include<vector>
#include<utility>
#include<algorithm>

#include "../bvh.hpp"
#include "../common/common.hpp"
#include "../scene.hpp"

constexpr double EPS = 1e-6;

TEST(aabbConstructor, nopara){
  AABB box;
  for(int i=0;i<3;++i)
    EXPECT_GT(box.Axis(i).begin, box.Axis(i).end); // An empty interval
}

TEST(rayaabbIntersection, hit){
  AABB box(Point3(-1,-1,-1), Point3(1,1,1));

  Ray r(Point3(0,-3,0), Point3(1,0,0)-Point3(0,-3,0));
  EXPECT_TRUE(box.Is_Hit_in_Interval(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();

  r = Ray(Point3(0,-2,0), Point3(1.999,0,0)-Point3(0,-2,0));
  EXPECT_TRUE(box.Is_Hit_in_Interval(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();

  r = Ray(Point3(3,-4,8), Point3(-3,4,-8));
  EXPECT_TRUE(box.Is_Hit_in_Interval(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();
}

TEST(rayaabbIntersection, notHit){
  AABB box(Point3(-0.5,-1,-1), Point3(0.5,1,1));

  Ray r(Point3(0,-2,0), Point3(2,0,0)-Point3(0,-2,0));
  EXPECT_FALSE(box.Is_Hit_in_Interval(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();

  r = Ray(Point3(2,2,2), Point3(1,1,1));
  EXPECT_FALSE(box.Is_Hit_in_Interval(r, Interval{0.001, infinity})) << r.Origin()<<' '<<r.Direction();

  r = Ray(Point3(5,5,5), Point3(-1,-1,-1));
  EXPECT_FALSE(box.Is_Hit_in_Interval(r, Interval{0.001, 3.25})) << r.Origin()<<' '<<r.Direction();
}

TEST(rayaabbIntersection, ParallelRayHit){
  AABB box(Point3(1,1,1), Point3(-1,-1,-1));

  Ray r(Point3(-3,0,0), Point3(-1,0,0));
  EXPECT_TRUE(box.Is_Hit_in_Interval(r, Interval<double>{-1e10,1e10})) << r.Origin()<<' '<<r.Direction();

  r = Ray(Point3(0,-3,0), Point3(0,1,0));
  EXPECT_TRUE(box.Is_Hit_in_Interval(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();

  r = Ray(Point3(0.5,0.5,3), Point3(0,0,-0.5));
  EXPECT_TRUE(box.Is_Hit_in_Interval(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();
}

TEST(rayaabbIntersection, ParallelRayNoHit){
  AABB box(Point3(1,1,1), Point3(-1,-1,-1));

  Ray r(Point3(-3,0,1.001), Point3(-1,0,0));
  EXPECT_FALSE(box.Is_Hit_in_Interval(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();

  r = Ray(Point3(2,-3,1), Point3(0,1,0));
  EXPECT_FALSE(box.Is_Hit_in_Interval(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();

  r = Ray(Point3(1.5,0.5,3), Point3(0,0,-0.5));
  EXPECT_FALSE(box.Is_Hit_in_Interval(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();
}

TEST(aabbComparer, canSortAABBs){
  AABB a(Point3(-1,0,0),Point3(5,0,0)),
    b(Point3(-2,0,0), Point3(4,0,0)),
    c(Point3(4,0,1),Point3(0,0,0));
  std::vector<AABB> arr{a,b,c};

  std::sort(arr.begin(), arr.end(), AABB::Componentbased_Comparer(0));
  EXPECT_NEAR(arr[0].Axis(0).begin, -2, EPS);
  EXPECT_NEAR(arr[1].Axis(0).begin, -1, EPS);
  EXPECT_NEAR(arr[2].Axis(0).begin, 0, EPS);
}


// Value-parameterized tests for bvh construction
using ::testing::TestWithParam;
using ::testing::Values;

typedef std::vector<std::pair<Point3, int>> dataFactory();

std::vector<std::pair<Point3,int>> GenerateSmallData(){
}
