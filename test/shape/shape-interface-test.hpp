#ifndef SHAPE_INTERFACE_TEST_H
#define SHAPE_INTERFACE_TEST_H

#include<gmock/gmock.h>

#include<util/util.hpp>
#include<shape.hpp>


class MockShape : public IShape{
public:
  MockShape() {}
  
  MOCK_METHOD(Hit_record, Hit, (const Ray& r, const Interval<double>& period),  (const, override));
  MOCK_METHOD(AABB, Get_Bbox, (), (const, override));
};


#endif
