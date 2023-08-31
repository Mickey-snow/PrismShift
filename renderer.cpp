#include "renderer.hpp"
#include "common/mat.hpp"
#include "common/color.hpp"

#include<vector>
#include<future>

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

void _render_sub_process(Mat canvas, Interval<int> row, Interval<int> col, const Camera::View_Info& view, const Vector3& cam_position, const int& samples_per_pixel);
int _finished_render_sub_process_count,_total_render_sub_process_count;
void Write_Color(Mat& canvas, const int& i,const int &j, Color pixel_color){
  pixel_color = Format_Color(pixel_color);
  canvas.at<cv::Vec3b>(i,j)[0] = static_cast<uchar>(pixel_color.z());
  canvas.at<cv::Vec3b>(i,j)[1] = static_cast<uchar>(pixel_color.y());
  canvas.at<cv::Vec3b>(i,j)[2] = static_cast<uchar>(pixel_color.x());
}
Mat Renderer::__Renderer_facade::Render() const{
  std::clog<<"\rStart rendering"<<std::flush;

  Camera::View_Info view = cam->Get_Initialize_View();
  Mat canvas(cam->image_height, cam->image_width, CV_8UC3);

  static const int _pixel_block_size = 128;

  _total_render_sub_process_count = _finished_render_sub_process_count = 0;
  std::vector<std::future<void>> futures;
  for(int j=0;j<cam->image_height;j+=_pixel_block_size)
    for(int i=0;i<cam->image_width;i+=_pixel_block_size){
      ++_total_render_sub_process_count;
      futures.push_back(std::async(_render_sub_process, canvas, Interval{j,std::min(j+_pixel_block_size,cam->image_height)}, Interval{i,std::min(i+_pixel_block_size,cam->image_width)}, view, cam->Position(), samples_per_pixel));
    }
  
  for(auto& fut : futures) fut.wait();

  return canvas;
}

void _render_sub_process(Mat canvas, Interval<int> row, Interval<int> col, const Camera::View_Info& view, const Vector3& cam_position, const int& samples_per_pixel){
  for(int j=row.begin;j<row.end;++j){
    for(int i=col.begin;i<col.end;++i){
      Color pixel_color = Color(0,0,0);

      for(const auto& sample : Rand_Pixel_Samples(view, j, i, samples_per_pixel)){
	auto ray_direction = sample - cam_position;
	Ray r(cam_position, ray_direction);
	pixel_color += Renderer::Instance()->Ray_Color(r,0) / samples_per_pixel;
      }

      Write_Color(canvas, j,i, pixel_color);
    }
  }
  std::clog<<"\rline "<<row<<" column "<<col<<" Done. ("
	   <<++_finished_render_sub_process_count<<" out of "<<_total_render_sub_process_count
	   <<")    "<<std::flush;
}

Color Renderer::__Renderer_facade::Ray_Color(const Ray& r, int current_recur_depth) const{
  if(current_recur_depth > max_recurrent_depth) return Color(0,0,0);
  
  Hit_record rec; 
  rec = world->Ray_Hit(r, Interval{1e-8,infinity});
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
