#include <gtest/gtest.h>

#include <shapes/3d/sphere.hpp>
#include <util/util.hpp>

#include <memory>
#include <vector>
#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

class SphereTest : public ::testing::Test {
 protected:
  inline static constexpr int N = 64;
  inline static constexpr auto EPS = 1e-6;

  static Point3 rand_point(double min = -10, double max = 10) {
    return Point3(random_double(min, max), random_double(min, max),
                  random_double(min, max));
  }

  SphereTest()
      : unit_sphere(Point3(0, 0, 0), 1),
        o(rand_point(7, 10)),
        r(random_double(1, 3)),
        rand_sphere(o, r) {}

  Sphere unit_sphere;
  Point3 o;
  double r;
  Sphere rand_sphere;
};

TEST_F(SphereTest, Bbox) {
  auto bbox = unit_sphere.GetBbox();
  EXPECT_EQ(bbox, AABB(Point3(-1, -1, -1), Point3(1, 1, 1)));

  bbox = rand_sphere.GetBbox();
  EXPECT_TRUE(bbox.Contains(o + Vector3(r, r, r)));
  EXPECT_TRUE(bbox.Contains(o - Vector3(r, r, r)));
}

TEST_F(SphereTest, Area) {
  EXPECT_EQ(unit_sphere.Area(), 4 * pi);
  EXPECT_EQ(rand_sphere.Area(), 4 * pi * r * r);
}

TEST_F(SphereTest, RayHits) {
  Point3 on_sphere, ray_point;
  HitRecord rec;
  Ray r;
  double time;

  for (auto* sphere : std::vector<Sphere*>{&unit_sphere, &rand_sphere}) {
    for (int i = 0; i < N; ++i) {
      ray_point = rand_point(0, .1);  // inside
      on_sphere = Point3(rand_sphere_uniform());
      r = Ray(sphere->GetTransformation().Doit(ray_point),
              sphere->GetTransformation().Doit(on_sphere - ray_point));
      time = r.direction.Length();
      r.direction /= time;

      rec = sphere->Hit(r, Interval<double>::Positive());
      EXPECT_TRUE(rec.hits) << ray_point << ' ' << on_sphere;
      EXPECT_NEAR(rec.time, time, EPS);

      // outside
      on_sphere = Point3(rand_sphere_uniform());
      ray_point = Point3(random_double(10, 20) * Vector3(on_sphere));
      r = Ray(sphere->GetTransformation().Doit(ray_point),
              sphere->GetTransformation().Doit(on_sphere - ray_point));
      time = r.direction.Length();
      r.direction /= time;

      rec = sphere->Hit(r, Interval<double>::Positive());
      EXPECT_TRUE(rec.hits) << ray_point << ' ' << on_sphere;
      EXPECT_NEAR(rec.time, time, EPS);
    }
  }
}

TEST_F(SphereTest, HitOutsideTimeInterval) {
  Point3 on_sphere, ray_point;
  HitRecord rec;
  Ray r;
  double time;

  for (auto* sphere : std::vector<Sphere*>{&unit_sphere, &rand_sphere}) {
    for (int i = 0; i < N; ++i) {
      ray_point = rand_point(0, .1);  // inside
      on_sphere = Point3(rand_sphere_uniform());
      r = Ray(sphere->GetTransformation().Doit(ray_point),
              sphere->GetTransformation().Doit(on_sphere - ray_point));
      time = r.direction.Length();
      r.direction /= time;

      rec = sphere->Hit(r, Interval<double>(0, time - EPS));
      EXPECT_FALSE(rec.hits) << ray_point << ' ' << on_sphere;

      // outside
      on_sphere = Point3(rand_sphere_uniform());
      ray_point = Point3(random_double(10, 20) * Vector3(on_sphere));
      r = Ray(sphere->GetTransformation().Doit(ray_point),
              sphere->GetTransformation().Doit(on_sphere - ray_point));
      time = r.direction.Length();
      r.direction /= time;

      rec = sphere->Hit(r, Interval<double>(0, time - EPS));
      EXPECT_FALSE(rec.hits) << ray_point << ' ' << on_sphere;
    }
  }
}

TEST_F(SphereTest, Nohit) {
  for (auto* sphere : std::vector<Sphere*>{&unit_sphere, &rand_sphere}) {
    for (int i = 0; i < N; ++i) {
      Point3 p = Point3(1.001 * Vector3::Random_Unit());
      Vector3 vp = Vector3::Cross((Vector3)p, Vector3::Random_Unit());
      p = sphere->GetTransformation().Doit(p);
      vp = sphere->GetTransformation().Doit(vp);

      Ray r = Ray(p - vp, vp);
      auto rec = sphere->Hit(r, Interval<double>::Universe());
      EXPECT_FALSE(rec.hits) << r;
    }
  }
}
