#ifndef RENDERER_PANEL_H
#define RENDERER_PANEL_H

#include<memory>
#include<util/util.hpp>

class Camera;
class Scene;
class Renderer{
  public:
  Renderer() = default;
  ~Renderer() = default;
  
    void Set_Camera(std::shared_ptr<Camera> cam_){ cam=cam_; }
    void Set_World(std::shared_ptr<Scene> world_){ world=world_; }

    Mat Render();
    Color Ray_Color(const Ray&,int current_recur_depth) const;

    void Set_Samples_per_pixel(const int& samples){ samples_per_pixel = samples; }
    void Set_Max_recurrent_depth(const int& max_recur){ max_recurrent_depth = max_recur; }
    void Set_Preview_switch(const bool& show_preview){ show_preview_window = show_preview; }
    void Set_Threads(const int& threads){ max_threads = threads; }
  private:
    std::shared_ptr<Camera> cam;
    std::shared_ptr<Scene> world;
    
    // Render info
    int samples_per_pixel;
    int max_recurrent_depth;
    int max_threads;
    bool show_preview_window;
};

#endif
