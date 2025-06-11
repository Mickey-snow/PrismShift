#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "primitive.hpp"
#include "shape.hpp"
#include "shapes/3d/sphere.hpp"

#include "common/mshape.hpp"
#include "common/transformations.hpp"

#include <numbers>

using ::testing::_;
using ::testing::Return;

TEST(PrimitiveTest, BboxIsCachedAndTransformed) {
  auto shape = std::make_shared<mShape>();
  AABB base_box(Point3(-1, -2, -3), Point3(2, 3, 4));
  auto tr = MatrixTransformation::Translate(1, 2, 3) *
            MatrixTransformation::RotateZ(std::numbers::pi / 3);

  EXPECT_CALL(*shape, GetBbox()).Times(1).WillOnce(Return(base_box));

  Primitive prim(shape, nullptr, std::make_shared<MatrixTransformation>(tr));
  AABB expected = base_box.Transform(tr);
  EXPECT_EQ(prim.GetBbox(), expected);
  EXPECT_EQ(prim.GetBbox(), expected);  // ensure cached
}

TEST(PrimitiveTest, HitTransformsRayAndUndoHit) {
  auto shape = std::make_shared<mShape>();
  auto tr = MatrixTransformation::RotateY(0.5) *
            MatrixTransformation::Translate(3, -2, 1);

  Ray world_ray(Point3(5, 0, -3), Vector3(-1, 0.5, 2));
  Ray local_ray =
      world_ray.Transform(*std::make_shared<MatrixTransformation>(tr));
  HitRecord local_rec = HitRecord::RTN(local_ray, 2.0, Normal{0, 1, 0});

  EXPECT_CALL(*shape, GetBbox()).Times(1).WillOnce(Return(AABB()));
  EXPECT_CALL(
      *shape,
      Hit(::testing::Truly([&](const Ray& r) { return r == local_ray; }), _))
      .Times(1)
      .WillOnce(Return(local_rec));

  Primitive prim(shape, nullptr, std::make_shared<MatrixTransformation>(tr));

  auto rec = prim.Hit(world_ray, Interval<double>::Universe());
  EXPECT_TRUE(rec.hits);
  EXPECT_EQ(rec.hitted_obj, &prim);
  EXPECT_EQ(rec.ray, world_ray);
  EXPECT_DOUBLE_EQ(rec.time, 2.0);
  Point3 expected_pos = tr.Undo(local_rec.position);
  Normal expected_normal = tr.Undo(Normal{0, 1, 0});
  EXPECT_EQ(rec.position, expected_pos);
  EXPECT_EQ(rec.normal, expected_normal);
  EXPECT_EQ(rec.front_face,
            Vector3::Dot(world_ray.Direction(), expected_normal) < 0);
}

class PrimitiveSpherePropertyTest
    : public ::testing::TestWithParam<MatrixTransformation> {};

TEST_P(PrimitiveSpherePropertyTest, ConsistentWithManualTransform) {
  MatrixTransformation tr = GetParam();
  auto shape = std::make_shared<Sphere>();
  Primitive prim(shape, nullptr, std::make_shared<MatrixTransformation>(tr));

  AABB expected_box = shape->GetBbox().Transform(tr);
  EXPECT_EQ(prim.GetBbox(), expected_box);

  for (int i = 0; i < 50; ++i) {
    Ray r_world((Point3)Vector3::Random(-4, 4), Vector3::Random_Unit());
    Interval<double> interval = Interval<double>::Universe();

    auto rec = prim.Hit(r_world, interval);

    Ray local = r_world.Transform(tr);
    auto local_rec = shape->Hit(local, interval);

    if (!local_rec.hits) {
      EXPECT_FALSE(rec.hits);
      continue;
    }

    Point3 pos = tr.Undo(local_rec.position);
    Normal normal = tr.Undo(local_rec.normal);
    bool front = Vector3::Dot(r_world.Direction(), normal) < 0;

    EXPECT_TRUE(rec.hits);
    EXPECT_DOUBLE_EQ(rec.time, local_rec.time);
    EXPECT_EQ(rec.position, pos);
    EXPECT_EQ(rec.normal, normal);
    EXPECT_EQ(rec.front_face, front);
  }
}

INSTANTIATE_TEST_SUITE_P(PrimitiveSphereProps,
                         PrimitiveSpherePropertyTest,
                         ::testing::ValuesIn(my_transformations));
