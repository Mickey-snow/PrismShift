#include <gtest/gtest.h>

#include <shape.hpp>
#include <shapes/2d/parallelogram.hpp>
#include <shapes/2d/plane.hpp>
#include <shapes/2d/triangle.hpp>
#include <util/util.hpp>

#include <functional>
#include <string>
#include <type_traits>
#include <typeinfo>

using namespace std::string_literals;

using xyPlaneTypes = ::testing::Types<Plane, Parallelogram, Triangle>;

template <typename T>
class PlaneTest : public ::testing::Test {
 public:
  std::unique_ptr<IShape> shape = std::make_unique<T>();

  const double MIN = -1e5;
  const double MAX = 1e5;

  const int testcases = 100;

  std::function<Ray(double)> spawn_hit_ray = [this](const double t) {
    Point3 r_orig = (Point3)Vector3::Random(this->MIN, this->MAX);
    Point3 p_onplane;

    if constexpr (std::is_same_v<T, Plane>)
      p_onplane = Point3(random_double(this->MIN, this->MAX),
                         random_double(this->MIN, this->MAX), 0);
    else if constexpr (std::is_same_v<T, Parallelogram>)
      p_onplane = Point3(random_double(0, 1), random_double(0, 1), 0);
    else if constexpr (std::is_same_v<T, Triangle>) {
      p_onplane = Point3(random_double(0, 1), 0, 0);
      p_onplane.y() = random_double(0, 1 - p_onplane.x());
    } else
      throw;

    Vector3 r_dir = p_onplane - r_orig;
    return Ray(r_orig, r_dir / t);
  };
  std::function<Ray()> spawn_nohit_ray = [this]() {
    auto hit_ray = std::invoke(this->spawn_hit_ray, 1.0);
    return Ray(hit_ray.Origin(), -hit_ray.Direction());
  };
  std::function<Ray()> spawn_parallel_ray = [this]() {
    Point3 r_orig = (Point3)Vector3::Random(this->MIN, this->MAX);
    Vector3 r_dir = Vector3::Random_Unit();
    r_dir.z() = 0;
    return Ray(r_orig, r_dir);
  };
};
TYPED_TEST_SUITE(PlaneTest, xyPlaneTypes);

TYPED_TEST(PlaneTest, bbox) {
  auto box = this->shape->GetBbox();

  if (dynamic_cast<Plane*>(this->shape.get()))
    EXPECT_TRUE(box.Contains(AABB(Point3(this->MIN, this->MIN, 0),
                                  Point3(this->MAX, this->MAX, 0))));
  else if (dynamic_cast<Parallelogram*>(this->shape.get()))
    EXPECT_TRUE(box.Contains(AABB(Point3(0, 0, 0), Point3(1, 1, 1))));
  else if (dynamic_cast<Triangle*>(this->shape.get()))
    EXPECT_TRUE(box.Contains(AABB(Point3(0, 1, 0), Point3(1, 0, 0))));

  else
    FAIL() << "assertion logic not specified for type "s +
                  typeid(decltype(this->shape)).name();
}

TYPED_TEST(PlaneTest, rayHit) {
  for (int i = 0; i < this->testcases; ++i) {
    const double time = random_double(1, 10);
    Ray r = std::invoke(this->spawn_hit_ray, time);

    auto rec = this->shape->Hit(r, Interval<double>::Positive());
    ASSERT_TRUE(rec.hits);
    EXPECT_DOUBLE_EQ(rec.time, time);
    if (r.Origin().z() > 0)
      EXPECT_TRUE(rec.front_face);
    else
      EXPECT_FALSE(rec.front_face);
  }
}

TYPED_TEST(PlaneTest, rayHitOutInterval) {
  static constexpr double EPS = 1e-5;

  for (int i = 0; i < this->testcases; ++i) {
    const double time = random_double(1, 10);
    Ray r = std::invoke(this->spawn_hit_ray, time);

    auto rec = this->shape->Hit(r, Interval<double>(EPS, time - EPS));
    EXPECT_FALSE(rec.hits);
  }
}

TYPED_TEST(PlaneTest, rayNoHit) {
  for (int i = 0; i < this->testcases; ++i) {
    Ray r = std::invoke(this->spawn_nohit_ray);

    auto rec = this->shape->Hit(r, Interval<double>::Positive());
    EXPECT_FALSE(rec.hits);
  }
}

TYPED_TEST(PlaneTest, parallelRayNoHit) {
  for (int i = 0; i < this->testcases; ++i) {
    Ray r = std::invoke(this->spawn_parallel_ray);

    auto rec = this->shape->Hit(r, Interval<double>::Universe());
    EXPECT_FALSE(rec.hits);
  }
}
