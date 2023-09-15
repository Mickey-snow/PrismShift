#include "camera.hpp"
#include "util/util.hpp"


Camera::View_Info Camera::Get_Initialize_View() const{
  Camera::View_Info view;

  view.focal_length = (looking_at-camera_center).Length();
  
  // Viewport size
  auto theta = Degree2Rad(view_angle_vertical);
  auto h = tan(theta/2);
  view.viewport_height = 2*h*view.focal_length;
  view.viewport_width = view.viewport_height * (static_cast<double>(image_width)/image_height);

  // The camera coordinate frame basis vectors
  auto w = (looking_at-camera_center).Normalize(); // front
  auto u = Vector3::Cross(up_direction, w).Normalize();     // right
  auto v = Vector3::Cross(u,w);			      // down
  
  // Vectors across the horizontal and down the vertical viewport edges
  view.viewport_u = view.viewport_width * u;
  view.viewport_v = view.viewport_height * v;

  // The horizontal and vertical delta vectors from pixel to pixel
  view.pixel_delta_u = view.viewport_u / image_width;
  view.pixel_delta_v = view.viewport_v / image_height;

  // Location of the upper left pixel
  view.viewport_upper_left = camera_center + view.focal_length*w - view.viewport_u/2 - view.viewport_v/2;
  view.pixel00_loc = view.viewport_upper_left + 0.5*(view.pixel_delta_u+view.pixel_delta_v);

  return view;
}
