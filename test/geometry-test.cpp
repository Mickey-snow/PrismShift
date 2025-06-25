#include "util/vector.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>
#include <ranges>
#include <string>

static constexpr Float kEps = 1e-6;

TEST(geometryPrimitiveTest, BasicVector2) {
  Vector2 orig;
  Vector2 v1{5, -1}, v2{0.3, -1.2}, v3{10, 10};
  const Float scalar = 1.5, zero = 0;

  EXPECT_NEAR(v1.v[0], 5.0, kEps);
  EXPECT_NEAR(v1.v[1], -1.0, kEps);
  EXPECT_EQ(v1, Vector2(5.0, -1.0));

  EXPECT_EQ(v1 + v2, Vector2(5.3, -2.2));
  EXPECT_EQ(v2 + v3, Vector2(10.3, 8.8));

  EXPECT_EQ(v1 - v2, Vector2(4.7, 0.2));
  EXPECT_EQ(orig - v3, Vector2(-v3.x(), -v3.y()));

  EXPECT_EQ(v1 * scalar, Vector2(7.5, -1.5));
  EXPECT_EQ(scalar * v1, Vector2(7.5, -1.5));

  EXPECT_EQ(v1 / scalar, Vector2(10.0 / 3, -2.0 / 3));
  EXPECT_EQ(v2 / scalar, Vector2(0.2, -0.8));

  auto v = v1;
  (v *= scalar) += v2;
  EXPECT_EQ(v, Vector2(7.8, -2.7));

  for (const auto& v : {orig, v1, v2, v3}) {
    EXPECT_TRUE((v * zero).near_zero());
    EXPECT_TRUE((zero * v).near_zero());
  }

  EXPECT_NEAR(v1.Dot(v2), 2.7, kEps);
  EXPECT_NEAR(v3.Dot(orig), 0, kEps);

  EXPECT_NEAR(v1.Length_squared(), 26, kEps);
  EXPECT_NEAR(v2.Length_squared(), 1.53, kEps);
  EXPECT_NEAR(v3.Length_squared(), 200, kEps);
}

TEST(geometryPrimitiveTest, BasicVector3) {
  Vector3 v1, v2;
  v1 = Vector3{42, -12, 4};
  v2 = Vector3{0.4, 321, 8.9};
  Vector3 i{1, 0, 0}, j{0, 1, 0}, k{0, 0, 1};
  const Float scalar = 2.5;

  EXPECT_NEAR(v2[0], 0.4, kEps);
  EXPECT_NEAR(v2[1], 321.0, kEps);
  EXPECT_NEAR(v2[2], 8.9, kEps);
  EXPECT_EQ(v2, Vector3(0.4, 321, 8.9));

  EXPECT_EQ(v1 + v2, Vector3(42.4, 309, 12.9));
  EXPECT_EQ(v1 - v2, Vector3(41.6, -333, -4.9));
  EXPECT_EQ(v2 * scalar, Vector3(1, 802.5, 22.25));
  EXPECT_EQ(v1 / scalar, Vector3(16.8, -4.8, 1.6));

  EXPECT_EQ(Vector3::Cross(i, j), k);
  EXPECT_EQ(Vector3::Cross(v1, v2), Vector3(-1390.8, -372.2, 13486.8));
}

TEST(geometryPrimitiveTest, Normal) {
  const Float scale = 1.0 / pow(2.0, 1.0 / 3);
  EXPECT_EQ((Normal{1, 1, 0}), (Normal{scale, scale, 0}));
}

TEST(geometryPrimitiveTest, typesafe) {
  using vpv_t = decltype(std::declval<Vector2>() + std::declval<Vector2>());
  using vmv_t = decltype(std::declval<Vector2>() - std::declval<Vector2>());
  bool result =
      std::is_same_v<vpv_t, Vector2> && std::is_same_v<vmv_t, Vector2>;
  EXPECT_TRUE(result);

  using ppv_t = decltype(std::declval<Point2>() + std::declval<Vector2>());
  using vpp_t = decltype(std::declval<Vector2>() + std::declval<Point2>());
  result = std::is_same_v<ppv_t, Point2> && std::is_same_v<vpp_t, Point2>;
  EXPECT_TRUE(result);

  using pmp_t = decltype(std::declval<Point2>() - std::declval<Point2>());
  result = std::is_same_v<pmp_t, Vector2>;
  EXPECT_TRUE(result);

  using nmv_t = decltype(std::declval<Normal>() - std::declval<Vector3>());
  using npv_t = decltype(std::declval<Normal>() + std::declval<Vector3>());
  result = std::is_same_v<nmv_t, Normal> && std::is_same_v<npv_t, Normal>;
  EXPECT_TRUE(result);
}
