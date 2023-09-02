#include<gtest/gtest.h>

#include<global_config.hpp>

TEST(GlobalConfig, defaultConfig){
  EXPECT_TRUE(_Global::Instance()->show_render_window);
  EXPECT_EQ(_Global::Instance()->samples_per_pixel, 20);
  EXPECT_EQ(_Global::Instance()->max_recurrent_depth, 65);
  EXPECT_EQ(_Global::Instance()->image_height, 530);
  EXPECT_STREQ(_Global::Instance()->output_directory.c_str(), "out.ppm");
  EXPECT_TRUE(_Global::Instance()->use_timer);
  EXPECT_EQ(_Global::Instance()->max_threads, 25);
}

TEST(GlobalConfig, readExternalConfig){
  char arg0[] = "readexternalconfig-test";
  char arg1[] = "-nw";
  char arg2[] = "-spp", para2[] = "310";
  char arg3[] = "-mrd", para3[] = "110";
  char arg4[] = "-nt";
  char arg5[] = "-j", para5[] = "30";

  char* argv[] = {arg0,arg1,arg2,para2,arg3,para3,arg4,arg5,para5};
  int argc = 9;

  _Global::Instance()->Set_config(argc,argv);

  EXPECT_FALSE(_Global::Instance()->show_render_window);
  EXPECT_EQ(_Global::Instance()->samples_per_pixel, 310);
  EXPECT_EQ(_Global::Instance()->max_recurrent_depth, 110);
  EXPECT_EQ(_Global::Instance()->image_height, 530);
  EXPECT_STREQ(_Global::Instance()->output_directory.c_str(), "out.ppm");
  EXPECT_FALSE(_Global::Instance()->use_timer);
  EXPECT_EQ(_Global::Instance()->max_threads, 30);
}
