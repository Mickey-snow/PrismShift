#include <gtest/gtest.h>

#include <util/interval.hpp>

constexpr double EPS = 1e-8;

TEST(Mergeinterval, merge) {
  Interval a(-1000, 1000), b(-5, -3);
  Interval merged(b, a);
  EXPECT_NEAR(merged.begin, -1000.0, EPS);
  EXPECT_NEAR(merged.end, 1000.0, EPS);
}
