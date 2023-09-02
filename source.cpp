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
