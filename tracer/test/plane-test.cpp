#include<shapes/2d/plane.hpp>
#include<util/util.hpp>
#include<material.hpp>

#include "vec-comparer.hpp"
#include "constant.hpp"
#include<gtest/gtest.h>


class tPlane : public Plane{
public:
  using Plane::Plane;
  virtual std::pair<bool, Point3> Intersection(const Ray& r, const Interval<double>& time) const { return Plane::Intersection(r,time); }
  virtual std::pair<double,double> Decomposition(const Vector3& vec) const{
    return Plane::Decomposition(vec);
  }

  virtual bool On_Object(const double& a,const double& b) const{
    return Plane::On_Object(a,b);
  }

};

TEST(plane, intersection){
  auto plane = tPlane(Point3(0,0,0), Vector3(0,1,0), Vector3(1,0,1));
  auto r = Ray(Point3(0,0,-10), Point3(2.5,5,2.5)-Point3(0,0,-10));

  auto [hits,intersection] = plane.Intersection(r, Interval<double>::Universe());
  ASSERT_TRUE(hits);
  EXPECT_TRUE(Vec3eq(intersection, 2.5,5,2.5));
}

TEST(plane, decomposition){
  auto Q=Point3(0,0,0), u=Vector3(3.4,-0.9,2.8), v=Vector3(1,7,0);
  auto plane = tPlane(Q,u,v);

  auto a=3.2,b=9.1;
  auto P = a*u + b*v;
  auto [beta,alpha] = plane.Decomposition(P);
  EXPECT_NEAR(alpha, a, EPS);
  EXPECT_NEAR(beta, b, EPS);
}
