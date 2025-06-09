#pragma once

#include "shape.hpp"
#include "util/util.hpp"

class Camera {
 public:
  Camera(Point3 center = {},
         Point3 lookAt = {},
         int imageHeight = 1080,
         double aspectRatio = 16.0 / 9.0,
         double vFovDeg = 30.0) noexcept
      : camera_center_{std::move(center)},
        looking_at_{std::move(lookAt)},
        up_direction_{0, 1, 0},
        aspect_ratio_{aspectRatio},
        image_height_{imageHeight},
        image_width_{static_cast<int>(imageHeight * aspectRatio)},
        view_angle_vertical_{vFovDeg} {
    clampDimensions();
  }

  // Position & orientation
  void setPosition(Point3 p) noexcept { camera_center_ = std::move(p); }
  [[nodiscard]] Point3 position() const noexcept { return camera_center_; }

  void setLookAt(Point3 la) noexcept { looking_at_ = std::move(la); }
  [[nodiscard]] Point3 lookAt() const noexcept { return looking_at_; }

  void setAspectRatio(double ar) noexcept {
    aspect_ratio_ = ar;
    clampDimensions();
  }
  [[nodiscard]] double aspectRatio() const noexcept { return aspect_ratio_; }

  void setImageHeight(int h) noexcept {
    image_height_ = h;
    clampDimensions();
  }
  [[nodiscard]] int imageHeight() const noexcept { return image_height_; }

  void setImageWidth(int w) noexcept {
    image_width_ = w;
    clampDimensions();
  }
  [[nodiscard]] int imageWidth() const noexcept { return image_width_; }

  struct View_Info {
    double focal_length;
    double viewport_height, viewport_width;
    Vector3 viewport_u, viewport_v;
    Vector3 pixel_delta_u, pixel_delta_v;
    Point3 viewport_upper_left;
    Point3 pixel00_loc;
  };
  [[nodiscard]] View_Info initializeView() const;

 private:
  void clampDimensions() noexcept {
    image_height_ = std::max(1, image_height_);
    image_width_ = std::max(1, int(image_height_ * aspect_ratio_));
  }

  Point3 camera_center_;
  Point3 looking_at_;
  Vector3 up_direction_;
  double aspect_ratio_;
  int image_height_, image_width_;
  double view_angle_vertical_;
};
