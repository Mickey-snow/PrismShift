#include "renderer.hpp"
#include<scene.hpp>
#include<material.hpp>
#include<bsdf.hpp>
#include<shape.hpp>
#include<camera.hpp>

#include "opencv2/highgui.hpp"

#include<queue>
#include<future>

#include<shapes/3d/sphere.hpp>

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

void _update_preview_window(Mat canvas);
void _render_sub_process(const Renderer&, Mat, Interval<int> row, Interval<int> col, const Camera::View_Info&, const Vector3& cam_position, const int& samples_per_pixel);

int _finished_render_sub_process_count,_total_render_sub_process_count;
bool _show_preview_window;
void Write_Color(Mat& canvas, const int& i,const int &j, Color pixel_color){
  pixel_color = Format_Color(pixel_color, 1.0);
  canvas.at<cv::Vec3d>(i,j)[0] = pixel_color.z();
  canvas.at<cv::Vec3d>(i,j)[1] = pixel_color.y();
  canvas.at<cv::Vec3d>(i,j)[2] = pixel_color.x();
}


  // test metal bxdf ---
  class Metal : public Material{
  public:
    Metal(const Color& col) : color(col) {}
    virtual std::vector<std::shared_ptr<BxDF>> CalculateBSDF(const Hit_record& rec) override{
      return std::vector<std::shared_ptr<BxDF>>{
	std::make_shared<bsdf>(rec.normal,color)};
      }

  private:
    class bsdf : public BxDF{
    public:
      bsdf(Vector3 norm,Color col) : BxDF(BxDFType::Reflection|BxDFType::Specular),normal(norm),color(col) {}
      virtual Color f(const Vector3&, const Vector3&) const override{ return Color(0,0,0); }
      virtual std::tuple<Color,Vector3,double> Sample_f(const Vector3& rin) const override{
	const double pdf = 1.0;
	const Vector3 out_direction = Ray(Vector3(0,0,0), rin).Reflect_Direction(normal);
	return std::make_tuple(color, out_direction, pdf);
      }
      virtual double pdf(const Vector3&, const Vector3&) const override{ return 0.0; }
    private:
      Vector3 normal;
      Color color;
    };
    
    Color color;
  };
  // -----
Mat Renderer::Render(){


  std::shared_ptr<Material> material = std::make_shared<Metal>(Color(0.5,0.5,0.4));
  world->Add(std::make_shared<Sphere>(Point3(7,2,7), 2.0, material));
  world->Build_BVH();
  
  _show_preview_window = show_preview_window;
  if(_show_preview_window) cv::namedWindow("Preview", cv::WINDOW_AUTOSIZE);
  
  Camera::View_Info view = cam->Get_Initialize_View();
  Mat canvas(cam->image_height, cam->image_width, CV_64FC3);

  static const int _pixel_block_size = 64;
  static const int _max_subprocess_count = max_threads;

  _finished_render_sub_process_count = 0;
  _total_render_sub_process_count = ((cam->image_height-1)/_pixel_block_size+1) * ((cam->image_width-1)/_pixel_block_size+1);
  std::queue<std::future<void>> futures;

  std::clog<<"\rStart rendering"<<std::flush;
  for(int j=0;j<cam->image_height;j+=_pixel_block_size)
    for(int i=0;i<cam->image_width;i+=_pixel_block_size){
      while(futures.size() >= _max_subprocess_count){ futures.front().wait(); futures.pop(); }
      futures.push(std::async(_render_sub_process, *this, canvas, Interval{j,std::min(j+_pixel_block_size,cam->image_height)}, Interval{i,std::min(i+_pixel_block_size,cam->image_width)}, view, cam->Position(), samples_per_pixel));
    }
  
  while(!futures.empty()){ futures.front().wait(); futures.pop(); }

  cv::destroyAllWindows();
  return canvas;
}

void _render_sub_process(const Renderer& renderer, Mat canvas, Interval<int> row, Interval<int> col, const Camera::View_Info& view, const Vector3& cam_position, const int& samples_per_pixel){
  for(int j=row.begin;j<row.end;++j){
    for(int i=col.begin;i<col.end;++i){
      Color pixel_color = Color(0,0,0);

      for(const auto& sample : Rand_Pixel_Samples(view, j, i, samples_per_pixel)){
	auto ray_direction = sample - cam_position;
	Ray r(cam_position, ray_direction);
	pixel_color += renderer.Ray_Color(r,0) / samples_per_pixel;
      }

      Write_Color(canvas, j,i, pixel_color);
    }
    _update_preview_window(canvas);
  }
  std::clog<<"\rline "<<row<<" column "<<col<<" Done. ("
	   <<++_finished_render_sub_process_count<<" out of "<<_total_render_sub_process_count
	   <<")    "<<std::flush;

}

void _update_preview_window(Mat canvas){
  if(not _show_preview_window) return;
  imshow("Preview", canvas);
  cv::waitKey(1);
}


Color Renderer::Ray_Color(const Ray& r, int current_recur_depth) const{
  if(current_recur_depth > max_recurrent_depth) return Color(0,0,0);
  
  Hit_record rec = world->Ray_Hit(r, Interval<double>::Positive());
  if(!rec.hits) return Color(0,0,0); // background


  Color col = rec.hitted_obj->Get_Material()->Emission(rec);
  BSDF bsdf(rec.hitted_obj->Get_Material()->CalculateBSDF(rec));
  if(bsdf.bxdf_count >= 1){
    auto in_direction = r.Direction().Unit();
    auto [f,out_direction,pdf,flag] = bsdf.Sample_f(in_direction);

    double scatter_pdf = bsdf.pdf(in_direction, out_direction);
    if((flag&BxDFType::Specular) != BxDFType::None) scatter_pdf = 1;
    col += f*scatter_pdf*Ray_Color(Ray(rec.position,out_direction),
		     current_recur_depth + 1) / pdf;
  }
  
  return col;
}
