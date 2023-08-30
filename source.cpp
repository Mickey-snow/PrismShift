#include <opencv2/imgcodecs.hpp>

#include<iostream>

#include<chrono>

#include "common/common.hpp"
#include "scene.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "renderer.hpp"


signed main(void)
{
  const auto time_start = std::chrono::high_resolution_clock::now();
  
  // Add objects to the scene
  Scene world;
  world.Add(std::make_shared<Sphere>(Point3(0,0,-1), 0.5));
  world.Add(std::make_shared<Sphere>(Point3(0,-100.5,-1), 100));

  
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
