#include "renderer.hpp"
#include "common/mat.hpp"
#include "common/color.hpp"

#include<vector>

std::vector<Point3> Rand_Pixel_Samples(const Camera::View_Info& view, const int& row, const int& column, const int& total_samples){
  std::vector<Point3> samples;
  auto pixel_center = view.pixel00_loc + (column*view.pixel_delta_u) + (row*view.pixel_delta_v);

  for(int i=0;i<total_samples;++i){
    auto px = 0.5 - random_uniform_01();
    auto py = 0.5 - random_uniform_01();
    auto sample_square = px*view.pixel_delta_u + py*view.pixel_delta_v;
    samples.push_back(sample_square + pixel_center);
  }
  
  return samples;
}

void Write_Color(Mat& canvas, const int& i,const int &j, Color pixel_color){
  pixel_color = Format_Color(pixel_color);
  canvas.at<cv::Vec3b>(i,j)[0] = static_cast<uchar>(pixel_color.z());
  canvas.at<cv::Vec3b>(i,j)[1] = static_cast<uchar>(pixel_color.y());
  canvas.at<cv::Vec3b>(i,j)[2] = static_cast<uchar>(pixel_color.x());
}
Mat Renderer::__Renderer_facade::Render() const{
  Camera::View_Info view = cam->Get_Initialize_View();
  Mat canvas(cam->image_height, cam->image_width, CV_8UC3);

  for(int j=0;j<cam->image_height;++j){
    std::clog<<'\r'<<j+1<<" lines out of "<<cam->image_height<<" Done."<<std::flush;
    for(int i=0;i<cam->image_width;++i){
      Color pixel_color = Color(0,0,0);

      for(const auto& sample : Rand_Pixel_Samples(view, j, i, samples_per_frame)){
	auto ray_direction = sample - cam->Position();
	Ray r(cam->Position(), ray_direction);
	pixel_color += Ray_Color(r,0) / samples_per_frame;
      }

      Write_Color(canvas, j,i, pixel_color);
    }
  }

  return canvas;
}

Color Renderer::__Renderer_facade::Ray_Color(const Ray& r, int current_recur_depth) const{
  if(current_recur_depth > max_recurrent_depth) return Color(0,0,0);
  
  Hit_record rec; 
  rec = world->Ray_Hit(r, Interval{0,infinity});
  if(rec.hits){ 		// hits visible object
    rec.hit_counts = current_recur_depth + 1;
    return rec.hitted_obj->Ray_Color(r, rec);
  }

  // hits background
  // This can be later encapsulated in a global Ambient Light visible object
  Vector3 unit_direction = Unit_vector(r.Direction());
  auto a = 0.5*(unit_direction.y() + 1.0);
  return (1.0-a)*Color(1.0, 1.0, 1.0) + a*Color(0.5, 0.7, 1.0);
}