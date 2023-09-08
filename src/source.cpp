#include <opencv2/imgcodecs.hpp>
#include<argparse/argparse.hpp>

#include<iostream>
#include<string>
#include<memory>
#include<chrono>

#include "tracer/core.hpp"

argparse::ArgumentParser parser;
std::string output_file;
std::vector<std::string> input_files;
bool use_timer;

void Add_Arguments(){
  parser.add_argument("-i", "--input-files")
    .nargs(argparse::nargs_pattern::at_least_one);
  parser.add_argument("output_file")
    .required()
    .default_value("out.ppm");

  parser.add_argument("-nw", "--no-window")
    .help("no preview while rendering")
    .default_value(true)
    .implicit_value(false);
  parser.add_argument("-spp", "--samples-per-pixel")
    .default_value(30).scan<'i',int>();
  parser.add_argument("-mrd", "--max-recurrent-depth")
    .default_value(50).scan<'i',int>();
  parser.add_argument("-nt", "--no-timer")
    .default_value(true)
    .implicit_value(false);
  parser.add_argument("-j", "--threads")
    .default_value(16).scan<'i',int>();
}

void Parse_Arguments(int argc, char** argv){
  try{ parser.parse_args(argc, argv); }
  catch(const std::runtime_error& err){
    std::cerr << err.what() <<std::endl;
    std::exit(1);
  }
}

void Set_Config(){
  Renderer::Instance()->Set_Samples_per_pixel(parser.get<int>("-spp"));
  Renderer::Instance()->Set_Max_recurrent_depth(parser.get<int>("-mrd"));
  Renderer::Instance()->Set_Preview_switch(parser.get<bool>("-nw"));
  Renderer::Instance()->Set_Threads(parser.get<int>("-j"));

  output_file = parser.get<decltype(output_file)>("output_file");
  input_files = parser.get<decltype(input_files)>("-i");
  use_timer = parser.get<decltype(use_timer)>("-nt");
}



int main(int argc, char* argv[])
{
  const auto time_start = std::chrono::high_resolution_clock::now();

  Add_Arguments();
  Parse_Arguments(argc,argv);
  Set_Config();

  std::shared_ptr<Scene> world = std::make_shared<Scene>();
  std::shared_ptr<Camera> camera;

  for(const auto& input_source : input_files){
    Importer ifs(input_source);
    
    world->Add(ifs.GetScene());
    if(ifs.GetCamera() != nullptr) camera = ifs.GetCamera();
  }

  if(camera == nullptr){
    std::cerr << "No camera provided" << std::endl;
    std::exit(1);
  }
  
  // Add objects to the scene
  Renderer::Instance()->Set_World(world);
  Renderer::Instance()->Set_Camera(camera);
  
  // Render
  auto canvas = Renderer::Instance()->Render();
  Mat output;
  canvas.convertTo(output, CV_8UC3, 256);
  cv::imwrite(output_file.c_str(), output);

  
  const auto time_end = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> time_elapsed = time_end - time_start;
  if(use_timer)
    std::cout<<"\nExecution time: "<<time_elapsed.count()<<'s'<<std::endl;
  
  return 0;
}
