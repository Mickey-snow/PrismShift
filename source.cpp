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

  Importer ifs("test.json");
  ifs.GetScene();

  // Add objects to the scene
  Renderer::Instance()->Set_World(ifs.GetScene());
  Renderer::Instance()->Set_Camera(ifs.GetCamera());
  
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
