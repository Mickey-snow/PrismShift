#ifndef CAMERA_H
#define CAMERA_H

#include "util/util.hpp"
#include "shape.hpp"
#include<iostream>

class Renderer;

class Camera{
public:
  Camera(const Point3& center_position = Point3(0,0,0),
	 const Point3& looking_at_ = Point3(0,0,0),
	 const int& image_height_ = 1080,
	 const double& aspect_ratio_ = 16.0/9.0,
	 const double& view_angle_vertical_ = 30):
    camera_center(center_position),
    looking_at(looking_at_),
    aspect_ratio(aspect_ratio_),
    image_height(image_height_),
    view_angle_vertical(view_angle_vertical_)
  {
    up_direction = Vector3(0,1,0);
    image_width = static_cast<int>(image_height * aspect_ratio);
  }
  
  void Set_Position(const Point3& position){
    camera_center = position;
  }
  Point3 Position(void) const{ return camera_center; }
  void Set_Looking_point(const Point3& _looking_at){ looking_at = _looking_at; }

  void Set_Aspect_ratio(const double& _aspect_ratio){ aspect_ratio = _aspect_ratio; }

  void Set_View_angle_vertical(const double& angle){ view_angle_vertical = angle; }
    
  void Set_Image_Width(const int& _width){
    image_width = _width;
    image_height = static_cast<int>(image_width / aspect_ratio);
    assert(image_width>=1 and image_height>=1);
  }
  void Set_Image_Height(const int& _height){
    image_height = _height;
    image_width = static_cast<int>(image_height * aspect_ratio);
    assert(image_width>=1 and image_height>=1);
  }

  struct View_Info{
    double focal_length;
    double viewport_height, viewport_width;
    Vector3 viewport_u, viewport_v;
    Vector3 pixel_delta_u, pixel_delta_v;
    Point3 viewport_upper_left;
    Point3 pixel00_loc;
  };

  View_Info Get_Initialize_View() const;
  
private:
  // Image and Camera config
  double aspect_ratio;
  int image_height, image_width;
  Point3 camera_center;
  Vector3 up_direction;
  Point3 looking_at;

  double view_angle_vertical;

  friend Renderer;
};


#endif
