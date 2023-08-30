#include <opencv2/imgcodecs.hpp>

#include<iostream>
#include<string>
#include<chrono>

#include "common/common.hpp"
#include "scene.hpp"
#include "material.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "renderer.hpp"

signed main(void)
{
  const auto time_start = std::chrono::high_resolution_clock::now();

  // Add materials
  Material* yellow_mat = new Lambertian(Color(0.8,0.8,0.0));
  Material* pink_mat  = new Lambertian(Color(0.7,0.3,0.3));
  Material* shinny = new Metal(Color(0.8,0.8,0.8), 0.3);
  Material* gold = new Metal(Color(0.8,0.6,0.2), 1.0);
				     
  
  // Add objects to the scene
  Scene world;
  world.Add(std::make_shared<Sphere>(Point3(0,0,-1), 0.5, pink_mat));
  world.Add(std::make_shared<Sphere>(Point3(0,-100.5,-1), 100, yellow_mat));
  world.Add(std::make_shared<Sphere>(Point3(-1,0,-1), 0.5, shinny));
  world.Add(std::make_shared<Sphere>(Point3(1,0,-1), 0.5, gold));
  
  Camera cam(Point3(0,0,0));
  cam.Set_Width(1920);

  Renderer::Instance()->Set_Camera(&cam);
  Renderer::Instance()->Set_World(&world);

  auto canvas = Renderer::Instance()->Render();
  cv::imwrite("img.png", canvas);

  const auto time_end = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> time_elapsed = time_end - time_start;
  std::cout<<"\nExecution time: "<<time_elapsed.count()<<'s'<<std::endl;
  
  return 0;
}
