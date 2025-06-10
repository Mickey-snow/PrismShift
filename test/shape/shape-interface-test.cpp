#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <common/mshape.hpp>
#include <common/transformations.hpp>

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Return;

class ConcreteShapeTest
    : public ::testing::TestWithParam<MatrixTransformation> {
 protected:
  void SetUp() override {
    mshape = new mShape();
    minvisible = new mShape();

    EXPECT_CALL(*minvisible, GetBbox)
        .Times(AnyNumber())
        .WillRepeatedly(Return(AABB()));
    EXPECT_CALL(*minvisible, Hit)
        .Times(AnyNumber())
        .WillRepeatedly(Return(HitRecord()));

    shape.Set_Shape(mshape);
    invisible.Set_Shape(minvisible);

    tr = GetParam();
    shape.Set_Frame(tr);
    invisible.Set_Frame(tr);
  }

  void TearDown() override {
    delete mshape;
    delete minvisible;
  }

  mShape *mshape, *minvisible;
  ConcreteShape shape, invisible;
  MatrixTransformation tr;
};

TEST_P(ConcreteShapeTest, forwardsBbox) {
  auto original_box = AABB(Point3(-5, -2, 3), Point3(0, 0, 10));
  EXPECT_CALL(*mshape, GetBbox).Times(1).WillOnce(Return(original_box));

  auto box = shape.GetBbox();
  EXPECT_EQ(box, original_box.Transform(tr));

  box = invisible.GetBbox();
  EXPECT_TRUE(box.isEmpty());
}

TEST_P(ConcreteShapeTest, bboxCalcOnce) {
  const auto shapeBox = AABB(Point3(-10, 0, 56), Point3(1.2, 0, 1234));
  EXPECT_CALL(*mshape, GetBbox).Times(1).WillOnce(Return(shapeBox));

  const int call_times = 10;
  for (int i = 0; i < call_times; ++i) {
    auto box = shape.GetBbox();
    EXPECT_EQ(box, shapeBox.Transform(tr));
  }
}

TEST_P(ConcreteShapeTest, forwardsHit) {
  auto r = Ray((Point3)Vector3::Random(-100, 100), Vector3::Random_Unit());
  auto t = Interval<double>::Positive();

  EXPECT_CALL(*mshape, Hit(r.Transform(tr), t))
      .Times(1)
      .WillOnce(Return(HitRecord()));

  auto rec = shape.Hit(r, t);
  EXPECT_FALSE(rec.hits);
}

TEST_P(ConcreteShapeTest, throwAtShapeisNull) {
  shape.Set_Shape(nullptr);
  auto r = Ray((Point3)Vector3::Random_Unit(), Vector3::Random_Unit());

  EXPECT_ANY_THROW(shape.Hit(r, Interval<double>::Universe()));
  EXPECT_TRUE(shape.GetBbox().isEmpty());
}

INSTANTIATE_TEST_SUITE_P(ConcreteShapeTr,
                         ConcreteShapeTest,
                         testing::ValuesIn(my_transformations));
