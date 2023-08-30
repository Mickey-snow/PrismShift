#include "camera.hpp"
#include "common/common.hpp"

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

Vector3 Camera::Pixel_Sample_Square() const{
  auto px = 0.5 - random_uniform_01();
  auto py = 0.5 - random_uniform_01();
  return px*pixel_delta_u + py*pixel_delta_v;
}
void Camera::Render(const Visible& world, std::ostream& out){

  this->Initialize_view();

  out<<"P3\n"<<image_width<<' '<<image_height<<"\n255\n";

  for(int j=0;j<image_height;++j){
    std::clog<<"\r"<<j+1<<" out of "<<image_height<<" lines done."<<std::flush;
    for(int i=0;i<image_width;++i){
      auto pixel_center = pixel00_loc + (i*pixel_delta_u) + (j*pixel_delta_v);
      Color pixel_color = Color(0,0,0);
      
      for(int sample_i=0;sample_i<samples_per_frame;++sample_i){
	auto pixel_sample = pixel_center + Pixel_Sample_Square();
	auto ray_direction = pixel_sample - camera_center;
	Ray r(camera_center, ray_direction);
	pixel_color += Ray_Color(r,world) / samples_per_frame;
      }

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
