#include <gtest/gtest.h>

#include <shape.hpp>
#include <shapes/2d/parallelogram.hpp>
#include <shapes/2d/plane.hpp>
#include <shapes/2d/triangle.hpp>
#include <util/util.hpp>

#include <string>

template <typename T>
class PlaneTest : public ::testing::Test {
 public:
  inline static constexpr int N = 8;
  inline static constexpr auto EPS = 1e-6;

  static inline Point3 rand_point(double min = -10, double max = 10) {
    return Point3(random_double(min, max), random_double(min, max),
                  random_double(min, max));
  }

  Point3 o = rand_point(), a = rand_point(), b = rand_point();
  Vector3 e1 = a - o, e2 = b - o;
  std::unique_ptr<T> shape = std::make_unique<T>(o, a, b);

  Point3 rand_on_plane() {
    if constexpr (std::same_as<T, Triangle>) {
      double s = random_double(0, 0.5), t = random_double(0, 0.5);
      return o + e1 * s + e2 * t;
    }
    if constexpr (std::same_as<T, Parallelogram>) {
      double s = random_uniform_01(), t = random_uniform_01();
      return o + e1 * s + e2 * t;
    }
    if constexpr (std::same_as<T, Plane>) {
      double s = random_double(-10, 10), t = random_double(-10, 10);
      return o + e1 * s + e2 * t;
    }
  }

  std::optional<Point3> rand_off_plane() {
    if constexpr (std::same_as<T, Triangle>) {
      while (true) {
        double s = random_double(-10, 10), t = random_double(-10, 10);
        if (!(0 <= s && 0 <= t && s + t <= 1))
          return o + e1 * s + e2 * t;
      }
    }
    if constexpr (std::same_as<T, Parallelogram>) {
      while (true) {
        double s = random_double(-10, 10), t = random_double(-10, 10);
        if (!(0 <= s && s <= 1 && 0 <= t && t <= 1))
          return o + e1 * s + e2 * t;
      }
    }
    if constexpr (std::same_as<T, Plane>) {
      return std::nullopt;
    }
  }
};

using xyPlaneTypes = ::testing::Types<Plane, Parallelogram, Triangle>;
TYPED_TEST_SUITE(PlaneTest, xyPlaneTypes);

TYPED_TEST(PlaneTest, Bbox) {
  auto box = this->shape->GetBbox();
  EXPECT_TRUE(box.Contains(this->o));
  EXPECT_TRUE(box.Contains(this->a));
  EXPECT_TRUE(box.Contains(this->b));
}

TYPED_TEST(PlaneTest, RayHit) {
  Point3 on_plane, ray_point;
  double time;
  Ray r;

  for (int i = 0; i < this->N; ++i) {
    on_plane = this->rand_on_plane();
    ray_point = this->rand_point();
    r = Ray(ray_point, on_plane - ray_point);
    time = r.direction.Length();
    r.direction /= time;

    auto rec = this->shape->Hit(r, Interval<double>::Positive());
    EXPECT_TRUE(rec.hits) << this->o << ' ' << this->a << ' ' << this->b << '\n'
                          << r;
    EXPECT_NEAR(rec.time, time, this->EPS);
  }
}

TYPED_TEST(PlaneTest, HitOutsideInterval) {
  Point3 on_plane, ray_point;
  double time;
  Ray r;

  for (int i = 0; i < this->N; ++i) {
    on_plane = this->rand_on_plane();
    ray_point = this->rand_point();
    r = Ray(ray_point, on_plane - ray_point);
    time = r.direction.Length();
    r.direction /= time;

    auto rec = this->shape->Hit(r, Interval<double>(0, time - this->EPS));
    ASSERT_FALSE(rec.hits);
  }
}

TYPED_TEST(PlaneTest, NoHit) {
  for (int i = 0; i < this->N; ++i) {
    auto off_plane = this->rand_off_plane();
    if (!off_plane.has_value())
      continue;

    Point3 ray_point = this->rand_point();
    Ray r(ray_point, (*off_plane - ray_point).Normalized());

    auto rec = this->shape->Hit(r, Interval<double>::Positive());
    EXPECT_FALSE(rec.hits);
  }
}

TYPED_TEST(PlaneTest, parallelRayNoHit) {
  for (int i = 0; i < this->N; ++i) {
    Vector3 parallel =
        random_uniform_01() * this->e1 + random_uniform_01() * this->e2;
    Ray r(this->rand_point(), parallel.Normalized());

    auto rec = this->shape->Hit(r, Interval<double>::Universe());
    EXPECT_FALSE(rec.hits);
  }
}
