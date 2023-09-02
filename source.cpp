#include <opencv2/imgcodecs.hpp>

#include<iostream>
#include<string>
#include<memory>
#include<chrono>

#include "tracer/core.hpp"

#include<global_config.hpp>

int main(int argc, char* argv[])
{

  const auto time_start = std::chrono::high_resolution_clock::now();

  _Global::Instance()->Set_config(argc, argv);

  Importer ifs("test.in");
  
  // Add objects to the scene
  std::shared_ptr<Scene> world = std::make_shared<Scene>(ifs.Get_objects());
  Renderer::Instance()->Set_World(world);

  std::shared_ptr<Camera> cam = ifs.Get_camera();
  Renderer::Instance()->Set_Camera(cam);
  /*
  auto ground_material = std::make_shared<Lambertian>(Color(0.5,0.5,0.5));
  world.Add(std::make_shared<Sphere>(Point3(0,-1000,0), 1000, ground_material));

  for(int a=-15;a<=15;a++)
    for(int b=-15;b<=15;b++){
      auto choose_mat = random_uniform_01();
      Point3 center(a+0.9*random_uniform_01(),0.2,b+0.9*random_uniform_01());

      if((center-Point3(4,0.2,0)).Length() > 0.9){
	std::shared_ptr<Material> sphere_material;

	if(choose_mat < 0.8){
	  sphere_material = std::make_shared<Lambertian>(Color::Random() * Color::Random());
	} else if(choose_mat < 0.95){
	  sphere_material = std::make_shared<Metal>(Color::Random(0.5,0.8), random_double(0, 0.5));
	} else{
	  sphere_material = std::make_shared<Dielectric>(1.5);
	}
	
	world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
      }
    }
  auto mat1 = std::make_shared<Dielectric>(1.5);
  auto mat2 = std::make_shared<Lambertian>(Color(0.4,0.2,0.1));
  auto mat3 = std::make_shared<Metal>(Color(0.7,0.6,0.5), 0.0);
  world.Add(std::make_shared<Sphere>(Point3(0,1,0),1.0, mat1));
  world.Add(std::make_shared<Sphere>(Point3(-4,1,0), 1.0, mat2));
  world.Add(std::make_shared<Sphere>(Point3(4,1,0), 1.0, mat3));

 
  // Set up camera
  Camera cam(Point3(13,2,3),Point3(0,0,0));
  Renderer::Instance()->Set_Camera(&cam);
  cam.Set_View_angle_vertical(20);
  cam.Set_Image_Height(_Global::Instance()->image_height);
  */
  
  // Render
  auto canvas = Renderer::Instance()->Render();
  Mat output;
  canvas.convertTo(output, CV_8UC3, 256);
  cv::imwrite(_Global::Instance()->output_directory.c_str(), output);

  const auto time_end = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> time_elapsed = time_end - time_start;
  if(_Global::Instance()->use_timer)
    std::cout<<"\nExecution time: "<<time_elapsed.count()<<'s'<<std::endl;
  
  return 0;
}
