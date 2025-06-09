#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <primitive.hpp>
#include <shape.hpp>

#include <common/mshape.hpp>

using testing::AnyNumber;
using testing::Return;

class PrimitiveTest : public ::testing::Test {
 protected:
  void SetUp() override {
    EXPECT_CALL(always_hit_shape, Hit)
        .Times(AnyNumber())
        .WillRepeatedly(Return(Hit_record::RTN(_dummyr, 0, Normal{0, 1, 0})));
    EXPECT_CALL(never_hit_shape, Hit)
        .Times(AnyNumber())
        .WillRepeatedly(Return(Hit_record{}));

    always_hit_s->Set_Shape(&always_hit_shape);
    never_hit_s->Set_Shape(&never_hit_shape);
  }

  const Point3 _dummyp = (Point3)Vector3::Random_Unit();
  const Vector3 _dummyv = Vector3::Random_Unit();
  const Ray _dummyr = Ray{_dummyp, _dummyv};
  const Normal _dummyn = (Normal)Vector3::Random_Unit();

  mShape always_hit_shape;
  mShape never_hit_shape;

  std::shared_ptr<ConcreteShape> always_hit_s =
      std::make_shared<ConcreteShape>();
  std::shared_ptr<ConcreteShape> never_hit_s =
      std::make_shared<ConcreteShape>();

  Primitive prim;
};

TEST_F(PrimitiveTest, recordsPrimitiveAtHit) {
  prim.Set_Shape(always_hit_s);
  auto rec = prim.Hit(_dummyr, Interval<double>::Positive());

  auto hitted_obj = dynamic_cast<Primitive const*>(rec.hitted_obj);
  ASSERT_TRUE(hitted_obj != nullptr);
  EXPECT_TRUE(hitted_obj == &prim);
}

TEST_F(PrimitiveTest, recordedPointerIsPrimitivePtr) {
  prim.Set_Shape(always_hit_s);
  auto rec = prim.Hit(_dummyr, Interval<double>::Positive());

  auto issame =
      std::is_same<decltype(rec.hitted_obj), IPrimitive const*>::value;
  EXPECT_TRUE(issame);
}

TEST_F(PrimitiveTest, recordsNullAtNoHit) {
  prim.Set_Shape(never_hit_s);
  auto rec = prim.Hit(_dummyr, Interval<double>::Universe());

  EXPECT_TRUE(rec.hitted_obj == nullptr);
  EXPECT_FALSE(rec.isHit());
}
