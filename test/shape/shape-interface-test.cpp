#include "shape-interface-test.hpp"

#include<gtest/gtest.h>
#include<gmock/gmock.h>

#include<common/transformations.hpp>

using ::testing::Return;
using ::testing::_;
using ::testing::AnyNumber;

class ConcreteShapeTest : public ::testing::TestWithParam<Transformation>{
protected:
  void SetUp() override{
    mshape = new MockShape();
    minvisible = new MockShape();

    EXPECT_CALL(*minvisible, Get_Bbox)
      .Times(AnyNumber())
      .WillRepeatedly(Return(AABB()));
    EXPECT_CALL(*minvisible, Hit)
      .Times(AnyNumber())
      .WillRepeatedly(Return(Hit_record::NoHit()));

    shape.Set_Shape(mshape);
    invisible.Set_Shape(minvisible);

    tr = GetParam();
    shape.Set_Frame(tr); invisible.Set_Frame(tr);
  }

  void TearDown() override{
    delete mshape;
    delete minvisible;
  }

  MockShape *mshape, *minvisible;
  ConcreteShape shape,invisible;
  Transformation tr;
};

TEST_P(ConcreteShapeTest, forwardsBbox){  
  auto original_box = AABB(Point3(-5,-2,3), Point3(0,0,10));
  EXPECT_CALL(*mshape, Get_Bbox).Times(1).WillOnce(Return(original_box));

  auto box = shape.Get_Bbox();
  EXPECT_EQ(box, original_box.Transform(tr));
  
  box = invisible.Get_Bbox();
  EXPECT_TRUE(box.isEmpty());
}

TEST_P(ConcreteShapeTest, bboxCalcOnce){
  const auto shapeBox = AABB(Point3(-10,0,56), Point3(1.2,0,1234));
  EXPECT_CALL(*mshape, Get_Bbox).Times(1).WillOnce(Return(shapeBox));

  const int call_times = 10;
  for(int i=0;i<call_times;++i){
    auto box = shape.Get_Bbox();
    EXPECT_EQ(box, shapeBox.Transform(tr));
  }
}

TEST_P(ConcreteShapeTest, forwardsHit){
  auto r = Ray((Point3)Vector3::Random(-100,100), Vector3::Random_Unit());
  auto t = Interval<double>::Positive();

  EXPECT_CALL(*mshape, Hit(r.Transform(tr), t))
    .Times(1)
    .WillOnce(Return(Hit_record::NoHit()));

  auto rec = shape.Hit(r,t);
  EXPECT_FALSE(rec.hits);
}

INSTANTIATE_TEST_SUITE_P(ConcreteShapeTr, ConcreteShapeTest,
			 testing::ValuesIn(my_transformations));
