#include <opencv2/imgcodecs.hpp>

#include<iostream>
#include<string>
#include<chrono>

#include "tracer/common/common.hpp"
#include "tracer/scene.hpp"
#include "tracer/material.hpp"
#include "tracer/sphere.hpp"
#include "tracer/camera.hpp"
#include "tracer/renderer.hpp"

signed main(void)
{
  const auto time_start = std::chrono::high_resolution_clock::now();

  // Add objects to the scene
  Scene world;
  Renderer::Instance()->Set_World(&world);

  auto ground_material = new Lambertian(Color(0.5,0.5,0.5));
  world.Add(std::make_shared<Sphere>(Point3(0,-1000,0), 1000, ground_material));

  for(int a=-15;a<=15;a++)
    for(int b=-15;b<=15;b++){
      auto choose_mat = random_uniform_01();
      Point3 center(a+0.9*random_uniform_01(),0.2,b+0.9*random_uniform_01());

      if((center-Point3(4,0.2,0)).Length() > 0.9){
	Material* sphere_material;

	if(choose_mat < 0.8){
	  sphere_material = new Lambertian(Color::Random() * Color::Random());
	} else if(choose_mat < 0.95){
	  sphere_material = new Metal(Color::Random(0.5,0.8), random_double(0, 0.5));
	} else{
	  sphere_material = new Dielectric(1.5);
	}
	
	world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
      }
    }
  auto mat1 = new Dielectric(1.5);
  auto mat2 = new Lambertian(Color(0.4,0.2,0.1));
  auto mat3 = new Metal(Color(0.7,0.6,0.5), 0.0);
  world.Add(std::make_shared<Sphere>(Point3(0,1,0),1.0, mat1));
  world.Add(std::make_shared<Sphere>(Point3(-4,1,0), 1.0, mat2));
  world.Add(std::make_shared<Sphere>(Point3(4,1,0), 1.0, mat3));

 
  // Set up camera
  Camera cam(Point3(13,2,3),Point3(0,0,0));
  Renderer::Instance()->Set_Camera(&cam);
  cam.Set_View_angle_vertical(20);
  cam.Set_Image_Height(580);

  // Render config
  Renderer::Instance()->Set_Samples_per_pixel(35);
  Renderer::Instance()->Set_Max_recurrent_depth(110);

  // Render
  auto canvas = Renderer::Instance()->Render();
  Mat output;
  canvas.convertTo(output, CV_8UC3, 256);
  cv::imwrite("img.png", output);

  const auto time_end = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> time_elapsed = time_end - time_start;
  std::cout<<"\nExecution time: "<<time_elapsed.count()<<'s'<<std::endl;
  
  return 0;
}
