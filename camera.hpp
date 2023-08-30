#ifndef CAMERA_H
#define CAMERA_H

#include "common.hpp"

#include<iostream>

class Camera{
public:
  Camera(const Point3& center_position = Point3(0,0,0),
	 const int& width = 1920,
	 const double& aspect_ratio_ = 16.0/9.0,
	 const double& focal_length_ = 1.0,
	 const double& viewpoint_height_ = 2.0) :
    aspect_ratio(aspect_ratio_),
    image_width(width),
    camera_center(center_position),
    focal_length(focal_length_),
    viewpoint_height(viewpoint_height_)
  {
    image_height = static_cast<int>(image_width / aspect_ratio);
    viewpoint_width = viewpoint_height * (static_cast<double>(image_width)/image_height);
  }
  
  void Render(const Visible& world,std::ostream& out=std::cout);

  void Set_Position(const Vector3& position){
    camera_center = position;
  }
  void Adjust_Position(const Vector3& translate){
    camera_center += translate;
  }

  void Set_Width(const int& _width){
    image_width = _width;
    image_height = static_cast<int>(image_width / aspect_ratio);
    assert(image_width>=1 and image_height>=1);
  }

private:
  // Image and Camera config
  double aspect_ratio;
  int image_width, image_height;
  double focal_length;
  double viewpoint_height, viewpoint_width;
  Point3 camera_center;

  // View info
  Vector3 viewport_u, viewport_v;
  Vector3 pixel_delta_u, pixel_delta_v;
  Point3 viewport_upper_left;
  Point3 pixel00_loc;

  void Initialize_view();

  Color Ray_Color(const Ray&, const Visible&) const;
};


#endif