#include <gtest/gtest.h>

#include "camera.hpp"
#include "util/prshdefs.hpp"

static constexpr Float kEps = 1e-7;

TEST(CameraTest, DefaultInitialization) {
  Camera cam;
  EXPECT_EQ(cam.imageHeight(), 1080);
  EXPECT_NEAR(cam.aspectRatio(), 16.0 / 9.0, kEps);
  int expectedWidth = static_cast<int>(1080 * (16.0 / 9.0));
  EXPECT_EQ(cam.imageWidth(), expectedWidth);
}

TEST(CameraTest, SetImageHeightAdjustsWidth) {
  Camera cam;
  cam.setImageHeight(720);
  EXPECT_EQ(cam.imageHeight(), 720);
  int expectedWidth = static_cast<int>(720 * cam.aspectRatio());
  EXPECT_EQ(cam.imageWidth(), expectedWidth);
}

TEST(CameraTest, SetImageWidthAdjustsHeight) {
  Camera cam;
  cam.setImageWidth(1920);
  EXPECT_EQ(cam.imageWidth(), 1920);
  int expectedHeight = static_cast<int>(1920 / cam.aspectRatio());
  EXPECT_EQ(cam.imageHeight(), expectedHeight);
}

TEST(CameraTest, SetAspectRatioRecomputesDimensions) {
  Camera cam;
  cam.setImageHeight(600);
  cam.setAspectRatio(4.0 / 3.0);
  EXPECT_NEAR(cam.aspectRatio(), 4.0 / 3.0, kEps);
  EXPECT_EQ(cam.imageHeight(), 600);
  int expectedWidth = static_cast<int>(600 * (4.0 / 3.0));
  EXPECT_EQ(cam.imageWidth(), expectedWidth);
}

TEST(CameraTest, ClampDimensionsMinValue) {
  Camera cam;
  cam.setAspectRatio(2.0);
  cam.setImageHeight(0);

  EXPECT_GE(cam.imageHeight(), 1) << "Height should clamp to at least 1";
  EXPECT_GE(cam.imageWidth(), 1) << "Width should also be at least 1";

  cam.setImageWidth(0);
  EXPECT_GE(cam.imageWidth(), 1);
  EXPECT_GE(cam.imageHeight(), 1);
}

TEST(CameraTest, PositionAndLookAtSetters) {
  Camera cam;
  Point3 p{1.0, 2.0, 3.0};
  cam.setPosition(p);
  EXPECT_EQ(cam.position(), p);

  Point3 look{4.0, 5.0, 6.0};
  cam.setLookAt(look);
  EXPECT_EQ(cam.lookAt(), look);
}

TEST(CameraTest, InitializeViewCustomOrientationAndFOV) {
  // Camera at origin looking along negative Z, FOV 90 degrees
  Camera cam(Point3{0, 0, 0}, Point3{0, 0, -1}, 100, 1.0, 90.0);
  auto view = cam.initializeView();

  // focal_length should be distance = 1
  EXPECT_NEAR(view.focal_length, 1.0, kEps);
  // viewport_height = 2 * tan(90/2°) * f = 2 * tan(45°) = 2
  EXPECT_NEAR(view.viewport_height, 2.0, kEps);
  // viewport_width = viewport_height * (width/height) = 2 * (100/100) = 2
  EXPECT_NEAR(view.viewport_width, 2.0, kEps);

  // pixel deltas should be viewport dims / 100
  EXPECT_NEAR(view.pixel_delta_u.x(), -view.viewport_width / 100.0, kEps);
  EXPECT_NEAR(view.pixel_delta_v.y(), -view.viewport_height / 100.0, kEps);
}

TEST(CameraTest, Pixel00LocationRelativeToUpperLeft) {
  Camera cam(Point3{0, 1, 2}, Point3{0, 1, 3}, 200, 2.0, 60.0);
  auto view = cam.initializeView();
  Vector3 deltaSum = view.pixel_delta_u + view.pixel_delta_v;
  Vector3 diff = view.pixel00_loc - view.viewport_upper_left;
  EXPECT_NEAR(diff.x(), 0.5 * deltaSum.x(), kEps);
  EXPECT_NEAR(diff.y(), 0.5 * deltaSum.y(), kEps);
  EXPECT_NEAR(diff.z(), 0.5 * deltaSum.z(), kEps);
}
