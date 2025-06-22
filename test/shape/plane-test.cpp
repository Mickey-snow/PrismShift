#include <gtest/gtest.h>

#include <shape.hpp>
#include <shapes/2d/parallelogram.hpp>
#include <shapes/2d/plane.hpp>
#include <shapes/2d/triangle.hpp>
#include <util/util.hpp>

#include <string>

using namespace vec_helpers;

template <typename T>
class PlaneTest : public ::testing::Test {
 public:
  inline static constexpr int N = 8;
  inline static constexpr auto EPS = 1e-6;

  static inline Point3 rand_point(Float min = -10, Float max = 10) {
    return Point3(random_double(min, max), random_double(min, max),
                  random_double(min, max));
  }

  Point3 o = rand_point(), a = rand_point(), b = rand_point();
  Vector3 e1 = a - o, e2 = b - o;
  std::unique_ptr<T> shape = std::make_unique<T>(o, a, b);

  Float u, v;

  Point3 rand_on_plane() {
    if constexpr (std::same_as<T, Triangle>) {
      v = random_double(0, 0.5), u = random_double(0, 0.5);
      return o + e1 * u + e2 * v;
    }
    if constexpr (std::same_as<T, Parallelogram>) {
      v = random_uniform_01(), u = random_uniform_01();
      return o + e1 * u + e2 * v;
    }
    if constexpr (std::same_as<T, Plane>) {
      v = random_double(-10, 10), u = random_double(-10, 10);
      return o + e1 * u + e2 * v;
    }
  }

  std::optional<Point3> rand_off_plane() {
    if constexpr (std::same_as<T, Triangle>) {
      while (true) {
        v = random_double(-10, 10), u = random_double(-10, 10);
        if (!(0 <= v && 0 <= u && v + u <= 1))
          return o + e1 * u + e2 * v;
      }
    }
    if constexpr (std::same_as<T, Parallelogram>) {
      while (true) {
        v = random_double(-10, 10), u = random_double(-10, 10);
        if (!(0 <= v && v <= 1 && 0 <= u && u <= 1))
          return o + e1 * u + e2 * v;
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
  static const Interval<Float> unit(0, 1);

  Point3 on_plane, ray_point;
  Float time;
  Ray r;

  for (int i = 0; i < this->N; ++i) {
    on_plane = this->rand_on_plane();
    ray_point = this->rand_point();
    r = Ray(ray_point, on_plane - ray_point);
    time = r.direction.Length();
    r.direction /= time;

    HitRecord rec = this->shape->Hit(r, Interval<Float>::Positive());
    EXPECT_TRUE(rec.hits) << this->o << ' ' << this->a << ' ' << this->b << '\n'
                          << r;
    EXPECT_NEAR(rec.time, time, this->EPS);
    EXPECT_EQ(rec.position, on_plane);
    Normal n = rec.normal;
    EXPECT_TRUE(IsPerpendicular(n, this->e1) && IsPerpendicular(n, this->e2));
    Float u = this->u, v = this->v;
    EXPECT_TRUE(unit.Contains(rec.uv.x()) && unit.Contains(rec.uv.y()))
        << rec.uv;
    EXPECT_EQ(rec.uv, Point2(u - std::floor(u), v - std::floor(v)));
  }
}

TYPED_TEST(PlaneTest, HitOutsideInterval) {
  Point3 on_plane, ray_point;
  Float time;
  Ray r;

  for (int i = 0; i < this->N; ++i) {
    on_plane = this->rand_on_plane();
    ray_point = this->rand_point();
    r = Ray(ray_point, on_plane - ray_point);
    time = r.direction.Length();
    r.direction /= time;

    auto rec = this->shape->Hit(r, Interval<Float>(0, time - this->EPS));
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

    auto rec = this->shape->Hit(r, Interval<Float>::Positive());
    EXPECT_FALSE(rec.hits);
  }
}

TYPED_TEST(PlaneTest, parallelRayNoHit) {
  for (int i = 0; i < this->N; ++i) {
    Vector3 parallel =
        random_uniform_01() * this->e1 + random_uniform_01() * this->e2;
    Ray r(this->rand_point(), parallel.Normalized());

    auto rec = this->shape->Hit(r, Interval<Float>::Universe());
    EXPECT_FALSE(rec.hits);
  }
}

TEST(TriangleTest, regression) {
  Point3 o(-0.744272, 0.62816, 0.773783), a(-0.757855, 0.62816, 0.765884),
      b(-0.771438, 0.62816, 0.757986);
  std::shared_ptr<IShape> tri;
  EXPECT_NO_THROW(tri = std::make_shared<Triangle>(o, a, b));
  EXPECT_NE(tri, nullptr);
}
