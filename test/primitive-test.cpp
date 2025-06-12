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

TEST(PrimitiveTest, HitTransformsRay) {
  auto shape = std::make_shared<mShape>();
  auto tr = MatrixTransformation::RotateY(0.5) *
            MatrixTransformation::Translate(3, -2, 1);

  Ray world_ray(Point3(5, 0, -3), Vector3(-1, 0.5, 2));
  Ray local_ray =
      world_ray.UndoTransform(*std::make_shared<MatrixTransformation>(tr));
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
  EXPECT_EQ(rec.primitive, &prim);
  EXPECT_EQ(rec.ray, world_ray);
  EXPECT_DOUBLE_EQ(rec.time, 2.0);
}
