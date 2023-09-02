#include<gtest/gtest.h>
#include<vector>
#include<utility>
#include<algorithm>

#include <bvh.hpp>
#include <util/util.hpp>
#include <scene.hpp>
#include <shapes/sphere.hpp>

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


namespace bvhDataTest{
// Value-parameterized tests for bvh construction
using ::testing::TestWithParam;
using ::testing::Values;

using Sphere_Data = std::vector<std::pair<Point3,int>>;
typedef std::vector<std::pair<Point3, int>> dataFactory();

Sphere_Data GenerateData(size_t data_size, Interval<int> value_range=Interval(-10,10)){
  Sphere_Data data;
  Point3 position;
  int radius;

  auto randint = [&value_range](){ return random_int(value_range.begin, value_range.end);};
  for(size_t i=0;i<data_size;++i){
    position = Point3(randint(), randint(), randint());
    radius = random_int(1,4);
    data.push_back(std::make_pair(position,radius));
  }

  return data;
}
Sphere_Data GenerateSmallData(){ return GenerateData(8); }
Sphere_Data GenerateScatteredData(){ return GenerateData(8, Interval<int>(-1000,1000)); }
Sphere_Data GenerateCondensedData(){ return GenerateData(128); }
Sphere_Data GenerateLargeData(){ return GenerateData(131072, Interval<int>(-2097152, 2097152)); }

class ValueParameterizedBVHFixture : public TestWithParam<dataFactory*>{
protected:
  
  void SetUp() override{
    for(const auto& it : ((*GetParam())()))
      spheres.push_back(std::make_shared<Sphere>(it.first, it.second));
  }

  void TearDown() override{
  }

  std::vector<std::shared_ptr<Shape>> spheres;
};

testing::AssertionResult HITRECORD_SAME(Hit_record expect, Hit_record actual){
  if(expect.hits != actual.hits)
    return testing::AssertionFailure() << "Miscalculate hits";
  if(!expect.hits) return testing::AssertionSuccess() << "Both report missed.";

  if(expect.time != actual.time)
    return testing::AssertionFailure() << "Miscalculate time. Expect: "<< expect.time << " Actual: "<<actual.time;

  return testing::AssertionSuccess() << "Aggrees in record.hits and record.time";
}
TEST_P(ValueParameterizedBVHFixture, BatRayHitwithBF){
  Scene world_with_bvh(spheres); world_with_bvh.Build_BVH();
  Scene world_without_bvh(spheres);

  Hit_record withbvh,withoutbvh;
  const int samples = 1000000/spheres.size();
  for(int i=0;i<samples;++i){
    Ray r(Vector3::Random(-175,-150), Vector3::Random_Unit());
    withbvh = world_with_bvh.Ray_Hit(r, Interval<double>::Positive());
    withoutbvh = world_without_bvh.Ray_Hit(r, Interval<double>::Positive());

    EXPECT_TRUE(HITRECORD_SAME(withbvh, withoutbvh));
  }
}

INSTANTIATE_TEST_SUITE_P(DataTestBVH, ValueParameterizedBVHFixture,
			 Values(&GenerateSmallData,
				&GenerateScatteredData,
				&GenerateCondensedData,
				&GenerateLargeData));
}
