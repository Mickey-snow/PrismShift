#ifndef RENDERER_PANEL_H
#define RENDERER_PANEL_H

#include<iostream>
#include<memory>

#include<util/util.hpp>
#include<scene.hpp>
#include<camera.hpp>
#include<material.hpp>

class Renderer{
private:
  class __Renderer_facade{
  public:
    __Renderer_facade(){}
    ~__Renderer_facade() = default;

    void Set_Camera(std::shared_ptr<Camera> cam_){ cam=cam_; }
    void Set_World(std::shared_ptr<Scene> world_){ world=world_; }

    Mat Render();
    Color Ray_Color(const Ray&,int current_recur_depth) const;

    void Set_Samples_per_pixel(const int& samples){ samples_per_pixel = samples; }
    void Set_Max_recurrent_depth(const int& max_recur){ max_recurrent_depth = max_recur; }

    void Set_global_illumin(std::shared_ptr<Material> light){ material = light; }
  private:
    std::shared_ptr<Camera> cam;
    std::shared_ptr<Scene> world;
    std::shared_ptr<Material> material;
    
    // Render info
    int samples_per_pixel;
    int max_recurrent_depth;
    bool show_preview_window;
  };

public:
  Renderer() = delete;
  Renderer(const Renderer&) = delete;

  static __Renderer_facade* Instance(void){
    static __Renderer_facade __control_panel_singleton;
    return &__control_panel_singleton;
  }
};

#endif
