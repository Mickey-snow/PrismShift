#include<gtest/gtest.h>

TEST(HelloTest, BasicAssertions){
  EXPECT_EQ(4*3, 12);
  EXPECT_STRNE("asdf", "asdf ");
}
