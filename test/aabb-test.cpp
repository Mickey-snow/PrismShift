#include<gtest/gtest.h>
#include<gmock/gmock.h>

#include<iterator>
#include<vector>
#include<utility>
#include<algorithm>
#include<functional>
#include<ranges>

#include<util/util.hpp>
#include<common/transformations.hpp>


class aabbTest : public ::testing::Test{
protected:
  void SetUp(){
    Interval<double> np1(-1,1);
    unit = AABB(np1,np1,np1);

    universe = AABB(Interval<double>::Universe(), Interval<double>::Universe(), Interval<double>::Universe());

    b1 = AABB(Point3(-0.5, -1, -1), Point3(0.5,1,1));
    b2 = AABB(Interval<double>::Universe(), Interval<double>::Empty(), Interval<double>::Universe());
  }

  AABB empty,universe;
  AABB unit;
  AABB b1,b2;

  testing::AssertionResult CheckIntersect(const AABB& box, const Ray& r, const Interval<double>& interval){
    if(box.isHitIn(r, interval))
      return testing::AssertionSuccess() << "Ray r=" << r << " hits bbox b=" << box << " in time interval " << interval;
    else return testing::AssertionFailure() << "Ray r=" << r << " missed bbox b=" << box << " in time interval " << interval;
  }
};

TEST_F(aabbTest, rayIntersectHit){
  Ray r;
  for(const auto& box : {unit, universe}){
    r = Ray(Point3(0,-3,0), Point3(1,0,0)-Point3(0,-3,0));
    EXPECT_TRUE(CheckIntersect(box, r, Interval<double>::Positive()));

    r = Ray(Point3(0,-2,0), Point3(1.999,0,0)-Point3(0,-2,0));
    EXPECT_TRUE(CheckIntersect(box, r, Interval<double>::Positive()));
  
    r = Ray(Point3(3,-4,8), Vector3(-3,4,-8));
    EXPECT_TRUE(CheckIntersect(box, r, Interval<double>::Positive()));
  }
}

TEST_F(aabbTest, rayIntersectNoHit){
  Ray r;
  for(const auto& box : {b1, b2}){
    r = Ray(Point3(0,-2,0), Point3(2,0,0)-Point3(0,-2,0));
    EXPECT_FALSE(box.isHitIn(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();

    r = Ray(Point3(2,2,2), Vector3(1,1,1));
    EXPECT_FALSE(box.isHitIn(r, Interval{0.001, infinity})) << r.Origin()<<' '<<r.Direction();

    r = Ray(Point3(5,5,5), Vector3(-1,-1,-1));
    EXPECT_FALSE(box.isHitIn(r, Interval{0.001, 3.25})) << r.Origin()<<' '<<r.Direction();
  }
}

TEST_F(aabbTest, parallelRayHit){
  const AABB& box = unit;

  Ray r(Point3(-3,0,0), Vector3(-1,0,0));
  EXPECT_TRUE(box.isHitIn(r, Interval<double>{-1e10,1e10})) << r.Origin()<<' '<<r.Direction();

  r = Ray(Point3(0,-3,0), Vector3(0,1,0));
  EXPECT_TRUE(box.isHitIn(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();
  r = Ray(Point3(0.5,0.5,3), Vector3(0,0,-0.5));
  EXPECT_TRUE(box.isHitIn(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();
}

TEST_F(aabbTest, parallelRayNoHit){
  const AABB& box = unit;

  Ray r(Point3(-3,0,1.001), Vector3(-1,0,0));
  EXPECT_FALSE(box.isHitIn(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();

  r = Ray(Point3(2,-3,1), Vector3(0,1,0));
  EXPECT_FALSE(box.isHitIn(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();

  r = Ray(Point3(1.5,0.5,3), Vector3(0,0,-0.5));
  EXPECT_FALSE(box.isHitIn(r, Interval<double>::Positive())) << r.Origin()<<' '<<r.Direction();
}

TEST(aabbComparer, canSortAABBs){
  AABB a(Point3(-1,0,0),Point3(5,0,0)),
    b(Point3(-2,0,0), Point3(4,0,0)),
    c(Point3(4,0,1),Point3(0,0,0));
  std::vector<AABB> arr{a,b,c};

  std::sort(arr.begin(), arr.end(), AABB::Componentbased_Comparer(0));
  EXPECT_DOUBLE_EQ(arr[0].Axis(0).begin, -2);
  EXPECT_DOUBLE_EQ(arr[1].Axis(0).begin, -1);
  EXPECT_DOUBLE_EQ(arr[2].Axis(0).begin, 0);
}


using ::testing::TestWithParam;
using ::testing::Values;


class aabbTransformTest : public TestWithParam<MatrixTransformation>{
protected:
  AABB square = AABB(Point3(0,0,0), Point3(1,1,0));
  AABB cube = AABB(Interval(-1.0,1.0),Interval(-1.0,1.0),Interval(-1.0,1.0));

  testing::AssertionResult is_transformed_inside(const std::vector<Point3>& v, AABB box){
    const MatrixTransformation tr = GetParam();
    for(const auto& it : v)
      if(not box.Contains(it))
	return testing::AssertionFailure() << "Point v=" << it<< " is not inside the bbox " << box << " before transformation!";

    box = box.Transform(tr);
    for(const auto& it : v | std::views::transform(tr))
      if(not box.Contains(it))
	return testing::AssertionFailure() << "Point v=" << it << " is not inside the bbox " << box << " after transformation";

    return testing::AssertionSuccess() << "All points are inside the bbox before and after the transformation";
  }
  testing::AssertionResult is_transformed_outside(const std::vector<Point3>& v, AABB box){
    const MatrixTransformation tr = GetParam();
    for(const auto& it : v)
      if(box.Contains(it))
	return testing::AssertionFailure() << "Point v=" << it << " is inside the bbox " << box << " before transformation!";

    box = box.Transform(tr);
    for(const auto& it : v | std::views::transform(tr))
      if(box.Contains(it))
	return testing::AssertionFailure() << "Point v=" << it << " is inside the bbox " << box << " after transformation";

    return testing::AssertionSuccess() << "All points are outside the bbox before and after the transformation";
  }
};

TEST_P(aabbTransformTest, faceCentersInsideCube){
  std::vector<Point3> v;
  for(int i=0;i<3;++i) for(auto j:{-1.0,1.0}){
      Point3 pt{0,0,0}; pt[i] = j; v.emplace_back(pt);
    }

  EXPECT_TRUE(is_transformed_inside(v, cube));
}

TEST_P(aabbTransformTest, verticesInsideCube){
  std::vector<Point3> v{Point3{0,0,0}};
  for(auto x:{-1.0,1.0}) for(auto y:{-1.0,1.0}) for(auto z:{-1.0,1.0})  v.emplace_back(Point3{x,y,z});
  EXPECT_TRUE(is_transformed_inside(v, cube));
}

TEST_P(aabbTransformTest, verticesInsideSquare){
  std::vector<Point3> v;
  for(auto x:{0.0,1.0}) for(auto y:{0.0,1.0}) v.emplace_back(Point3{x,y,0.0});
  EXPECT_TRUE(is_transformed_inside(v, square));
}

TEST_P(aabbTransformTest, approxInsideCube){
  std::vector<Point3> v;
  const int points_per_face = 100;
  for(int axis = 0;axis < 3;++axis)
    for(double i : {-1.0,1.0})
      std::generate_n(std::back_inserter(v), points_per_face, [&axis,&i](){
	Point3 p = (Point3)Vector3::Random(-1,1);
	p[axis] = i;
	return p;
      });
  EXPECT_TRUE(is_transformed_inside(v, cube));
}

TEST_P(aabbTransformTest, approxOutsideCube){
  const int points_per_face = 100;
  for(int axis = 0;axis < 3;++axis)
    for(double i : {-1.01,1.01})
      for(int cnt=0;cnt<points_per_face;++cnt){
	Point3 p = (Point3)Vector3::Random(-1,1);
	p[axis] = i;
	
	EXPECT_FALSE(cube.Contains(p));
      }
}

TEST_P(aabbTransformTest, approxInsideSquare){
  std::vector<Point3> v;
  const int points = 100;
  std::generate_n(std::back_inserter(v), points, [](){
    Point3 p = (Point3)Vector3::Random(0,1);
    p.z() = 0;
    return p;
  });
  EXPECT_TRUE(is_transformed_inside(v, square));
}

TEST_P(aabbTransformTest, approxOutsideSquare){
  const int points = 100;
  for(int cnt=0;cnt<points;++cnt){
    Point3 p = (Point3)Vector3::Random(0,1);
    p.z() = random_double(-1,1);

    EXPECT_FALSE(square.Contains(p));
  }
}


INSTANTIATE_TEST_SUITE_P(aabbtr, aabbTransformTest, testing::ValuesIn(my_transformations));
