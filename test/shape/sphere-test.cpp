#include <gtest/gtest.h>

#include <shape.hpp>
#include <shapes/3d/sphere.hpp>
#include <util/util.hpp>

#include <functional>
#include <vector>

class sphereTest : public ::testing::Test {
 protected:
  void SetUp() override {
    spawn_ray_fn = std::vector<std::function<Ray()>>{
        spawn_orig_directed_ray  // , spawn_orig_ray, spawn_tan_ray,
                                 // spawn_para_ray
    };
  }

  void TearDown() override { delete ball; }

  IShape const* ball = new Sphere();

  std::vector<std::function<Ray()>> spawn_ray_fn;
  std::function<Ray()> spawn_orig_directed_ray = []() {
    Point3 p_onball = (Point3)Vector3::Random_Unit();
    Point3 ray_orig = Point3(random_double(1.5, 10) * p_onball);
    Vector3 ray_d = p_onball - ray_orig;
    return Ray(ray_orig, ray_d);
  };
  std::function<Ray()> spawn_orig_ray = []() {
    return Ray(Point3{0, 0, 0}, Vector3::Random_Unit());
  };
  std::function<Ray()> spawn_para_ray = []() {
    const int axis = random_int(0, 3);
    Point3 p_onball = (Point3)Vector3::Random_Unit();
    Point3 ray_orig = p_onball;
    ray_orig[axis] = 1;
    Vector3 ray_d = p_onball - ray_orig;
    return Ray(ray_orig, ray_d);
  };
  std::function<Ray()> spawn_tan_ray = []() {
    Point3 ptan = (Point3)(0.999 * Vector3::Random_Unit());
    Vector3 vtan = Vector3::Cross((Vector3)ptan, Vector3::Random_Unit());
    return Ray(ptan - vtan, vtan);
  };
};

TEST_F(sphereTest, bbox) {
  auto bbox = ball->Get_Bbox();
  EXPECT_EQ(bbox, AABB(Point3(-1, -1, -1), Point3(1, 1, 1)));
}

TEST_F(sphereTest, rayHits) {
  static constexpr int testcases = 100;

  auto isInside = [](const Point3& p) { return p.Length_squared() <= 1.0; };

  for (const auto& spawn_fn : spawn_ray_fn) {
    for (int i = 0; i < testcases; ++i) {
      Ray r = spawn_fn();
      auto rec = ball->Hit(r, Interval<double>(0, 10.5));

      ASSERT_TRUE(rec.isHit());
      EXPECT_EQ(rec.ray, r);
      if (isInside(r.Origin()))
        EXPECT_FALSE(rec.front_face);
      else
        EXPECT_TRUE(rec.front_face);
    }
  }
}

TEST_F(sphereTest, hitOutTimeInterval) {
  static constexpr int testcases = 100;

  for (const auto& spawn_fn : spawn_ray_fn) {
    for (int i = 0; i < testcases; ++i) {
      Ray r = spawn_fn();
      auto rec = ball->Hit(r, Interval<double>(0.0001, 0.9999));
      EXPECT_FALSE(rec.isHit()) << r;
    }
  }
}

TEST_F(sphereTest, nohit) {
  static constexpr int testcases = 100;

  for (int i = 0; i < testcases; ++i) {
    Point3 p = (Point3)(1.001 * Vector3::Random_Unit());
    Vector3 vp = Vector3::Cross((Vector3)p, Vector3::Random_Unit());

    Ray r = Ray(p - vp, vp);
    auto rec = ball->Hit(r, Interval<double>::Universe());
    EXPECT_FALSE(rec.isHit()) << r;
  }
}
