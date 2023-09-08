#ifndef TEST_VECCOMPARER_H
#define TEST_VECCOMPARER_H

#include<gtest/gtest.h>

#include<util/util.hpp>


testing::AssertionResult Vec3eq(Vector3 vec, double x, double y,double z);
testing::AssertionResult Vec3eq(Vector3 vec1, Vector3 vec2);
testing::AssertionResult Vec2eq(Vector2 vec, double x,double y);
testing::AssertionResult Vec2eq(Vector2 vec1, Vector2 vec2);

#endif
