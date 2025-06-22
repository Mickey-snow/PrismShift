#include <gtest/gtest.h>

#include "mapping.hpp"
#include "util/util.hpp"

class SphericalMapTest
    : public ::testing::TestWithParam<std::shared_ptr<ITransformation>> {
 public:
  std::shared_ptr<ITransformation> trans;
  SphericalMap map;

  SphericalMapTest() : trans(GetParam()), map(trans) {}

  inline static constexpr int N = 16;
  static auto from_st(Point2 st) -> Vector3 {
    Float theta = st.y() * pi, phi = (st.x() - 0.5) * 2 * pi;
    return Vector3(std::sin(theta) * std::cos(phi), std::cos(theta),
                   std::sin(theta) * std::sin(phi));
  }
};

TEST_P(SphericalMapTest, RandomMap) {
  for (int i = 0; i < N; ++i) {
    Vector3 v = rand_sphere_uniform();

    Point3 p = trans->Doit(Point3(0, 0, 0) + v);
    Point2 st = map.Map(p);
    Vector3 vp = from_st(st);

    ASSERT_FALSE(vp.NearZero());
    EXPECT_EQ(v * (vp.Length() / v.Length()), vp);
  }
}

TEST_P(SphericalMapTest, EdgeCases) {
  for (int i = -1; i <= 1; ++i)
    for (int j = -1; j <= 1; ++j)
      for (int k = -1; k <= 1; ++k) {
        Vector3 v(i, j, k);
        if (v.NearZero())
          continue;

        Point3 p = trans->Doit(Point3(0, 0, 0) + v);
        Point2 st = map.Map(p);
        Vector3 vp = from_st(st);

        ASSERT_FALSE(vp.NearZero());
        EXPECT_EQ(v * (vp.Length() / v.Length()), vp);
      }
}

INSTANTIATE_TEST_SUITE_P(
    SphericalMapTests,
    SphericalMapTest,
    testing::Values(
        identity_transform,
        std::make_shared<VectorTranslate>(VectorTranslate::Translate(1, 2, 3)),
        std::make_shared<VectorScale>(VectorScale::Scale(3, 4, 5)),
        std::make_shared<MatrixTransformation>(
            MatrixTransformation::ChangeCoordinate(Point3(1, 1, 1),
                                                   Point3(4, 5, 6),
                                                   Point3(7, 8, 10)))));
