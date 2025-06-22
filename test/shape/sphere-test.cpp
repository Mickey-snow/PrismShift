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

  static Point3 rand_point(Float min = -10, Float max = 10) {
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
  Float r;
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
  EXPECT_NEAR(unit_sphere.Area(), 4 * pi, 1e-8);
  EXPECT_NEAR(rand_sphere.Area(), 4 * pi * r * r, 1e-8);
}

static auto from_uv(Point2 uv) -> Vector3 {
  Float theta = uv.y() * pi, phi = (uv.x() - 0.5) * 2 * pi;
  return Vector3(std::sin(theta) * std::cos(phi), std::cos(theta),
                 std::sin(theta) * std::sin(phi));
}
TEST_F(SphereTest, Uv) {
  // random
  for (int i = 0; i < 10; ++i) {
    Vector3 v = rand_sphere_uniform();

    Point2 uv = unit_sphere.GetUv(Point3(v));
    EXPECT_EQ(from_uv(uv), v);

    uv = rand_sphere.GetUv(Point3(v * r));
    EXPECT_EQ(from_uv(uv), v);
  }
}

TEST_F(SphereTest, UvEdgeCase) {
  static const Interval<Float> unit(0, 1);

  for (int i = -1; i <= 1; ++i)
    for (int j = -1; j <= 1; ++j)
      for (int k = -1; k <= 1; ++k) {
        Vector3 v(i, j, k);
        if (v.NearZero())
          continue;
        v = v.Normalized();

        Point2 uv = unit_sphere.GetUv(Point3(v));
        EXPECT_TRUE(unit.Contains(uv.x()) && unit.Contains(uv.y())) << uv;
        EXPECT_EQ(from_uv(uv), v);

        uv = rand_sphere.GetUv(Point3(v * r));
        EXPECT_EQ(from_uv(uv), v);
      }
}

TEST_F(SphereTest, RayHits) {
  Point3 on_sphere, ray_point;
  HitRecord rec;
  Ray r;
  Float time;

  for (auto* sphere : std::to_array<Sphere*>({&unit_sphere, &rand_sphere})) {
    for (int i = 0; i < N; ++i) {
      ray_point = rand_point(0, .1);  // inside
      on_sphere = Point3(rand_sphere_uniform() * sphere->r());
      r = Ray(sphere->GetTransformation().Doit(ray_point),
              sphere->GetTransformation().Doit(on_sphere - ray_point));
      time = r.direction.Length();
      r.direction /= time;

      rec = sphere->Hit(r, Interval<Float>::Positive());
      EXPECT_TRUE(rec.hits) << ray_point << ' ' << on_sphere;
      EXPECT_NEAR(rec.time, time, EPS);

      // outside
      on_sphere = Point3(rand_sphere_uniform() * sphere->r());
      ray_point = Point3(random_double(10, 20) * Vector3(on_sphere));
      r = Ray(sphere->GetTransformation().Doit(ray_point),
              sphere->GetTransformation().Doit(on_sphere - ray_point));
      time = r.direction.Length();
      r.direction /= time;

      rec = sphere->Hit(r, Interval<Float>::Positive());
      EXPECT_TRUE(rec.hits) << ray_point << ' ' << on_sphere;
      EXPECT_NEAR(rec.time, time, EPS);
    }
  }
}

TEST_F(SphereTest, HitOutsideTimeInterval) {
  Point3 on_sphere, ray_point;
  HitRecord rec;
  Ray r;
  Float time;

  for (auto* sphere : std::to_array<Sphere*>({&unit_sphere, &rand_sphere})) {
    for (int i = 0; i < N; ++i) {
      ray_point = rand_point(0, .1);  // inside
      on_sphere = Point3(rand_sphere_uniform() * sphere->r());
      r = Ray(sphere->GetTransformation().Doit(ray_point),
              sphere->GetTransformation().Doit(on_sphere - ray_point));
      time = r.direction.Length();
      r.direction /= time;

      rec = sphere->Hit(r, Interval<Float>(0, time - EPS));
      EXPECT_FALSE(rec.hits) << ray_point << ' ' << on_sphere;

      // outside
      on_sphere = Point3(rand_sphere_uniform() * sphere->r());
      ray_point = Point3(random_double(10, 20) * Vector3(on_sphere));
      r = Ray(sphere->GetTransformation().Doit(ray_point),
              sphere->GetTransformation().Doit(on_sphere - ray_point));
      time = r.direction.Length();
      r.direction /= time;

      rec = sphere->Hit(r, Interval<Float>(0, time - EPS));
      EXPECT_FALSE(rec.hits) << ray_point << ' ' << on_sphere;
    }
  }
}

TEST_F(SphereTest, Nohit) {
  for (auto* sphere : std::to_array<Sphere*>({&unit_sphere, &rand_sphere})) {
    for (int i = 0; i < N; ++i) {
      Point3 p = Point3(1.001 * Vector3::Random_Unit() * sphere->r());
      Vector3 vp = Vector3::Cross((Vector3)p, Vector3::Random_Unit());
      p = sphere->GetTransformation().Doit(p);
      vp = sphere->GetTransformation().Doit(vp);

      Ray r = Ray(p - vp, vp);
      auto rec = sphere->Hit(r, Interval<Float>::Universe());
      EXPECT_FALSE(rec.hits) << r;
    }
  }
}
