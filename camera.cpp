#include "camera.hpp"
#include "color.hpp"
#include "ray.hpp"

Color Camera::Ray_Color(const Ray& r, const Visible& world) const{
  Hit_record rec;

  rec = world.Ray_Hit(r, Interval{0,infinity});
  if(rec.hits){ 		// hits an visible object
    return rec.hitted_obj->Ray_Color(r, rec);
  }

  // background
  Vector3 unit_direction = Unit_vector(r.Direction());
  auto a = 0.5*(unit_direction.y() + 1.0);
  return (1.0-a)*Color(1.0, 1.0, 1.0) + a*Color(0.5, 0.7, 1.0);

}

void Camera::Render(const Visible& world, std::ostream& out){

  this->Initialize_view();

  out<<"P3\n"<<image_width<<' '<<image_height<<"\n255\n";

  for(int j=0;j<image_height;++j){
    for(int i=0;i<image_width;++i){
      auto pixel_center = pixel00_loc + (i*pixel_delta_u) + (j*pixel_delta_v);
      auto ray_direction = pixel_center-camera_center;
      Ray r(camera_center, ray_direction);
      
      Color pixel_color = Ray_Color(r,world);
      Write_Color(out, pixel_color);
    }
  }
}

void Camera::Initialize_view(){
  // Vectors across the horizontal and down the vertical viewport edges
  viewport_u = Vector3(viewpoint_width,0,0);
  viewport_v = Vector3(0, -viewpoint_height, 0);

  // The horizontal and vertical delta vectors from pixel to pixel
  pixel_delta_u = viewport_u / image_width;
  pixel_delta_v = viewport_v / image_height;

  // Location of the upper left pixel
  viewport_upper_left = camera_center - Vector3(0,0,focal_length) - viewport_u/2 - viewport_v/2;
  pixel00_loc = viewport_upper_left + 0.5*(pixel_delta_u+pixel_delta_v);

}
