#include<string>
#include<fstream>
#include<iostream>
#include<queue>
#include<future>

#include "common.hpp"
#include "scene.hpp"
#include "sphere.hpp"
#include "camera.hpp"

void Concurrent_body(Camera cam, Scene world, std::string filename){
  std::ofstream MyFile((filename).c_str());
  cam.Render(world,MyFile);
  MyFile.close();
  std::cout<<filename<<" Done."<<std::endl;
}

signed main(void)
{
  // Add objects to the scene
  Scene world;
  world.Add(std::make_shared<Sphere>(Point3(0,0,-1), 0.5));
  world.Add(std::make_shared<Sphere>(Point3(0,-100.5,-1), 100));

  
  Camera cam(Point3(0,0,0));
  cam.Set_Width(420);
  cam.Render(world, std::cout);
  return 0;
  
  cam.Set_Position(Point3(-1,0,0));
  double video_len = 3.0;
  double frame_rate = 60;
  double tot_frames = video_len * frame_rate;
  Vector3 translate(2,0,0);
  Vector3 translate_per_frame = translate/(frame_rate*video_len);

  const int threads = 16;
  std::queue<std::future<void>> thread_pool;
  for(int i=0;i<tot_frames;++i){
    std::string filename("image");
    filename += std::to_string(i) + ".ppm";

    if(thread_pool.size() < threads)
      thread_pool.push(std::async(Concurrent_body, cam, world, filename));
    else{
      thread_pool.front().wait();
      thread_pool.pop();
      thread_pool.push(std::async(Concurrent_body, cam,world,filename));
    }

    cam.Adjust_Position(translate_per_frame);
  }
  return 0;
}
