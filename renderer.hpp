#ifndef RENDERER_PANEL_H
#define RENDERER_PANEL_H

#include<iostream>

#include "common/common.hpp"
#include "objects.hpp"
#include "camera.hpp"

class Renderer{
private:
  class __Renderer_facade{
  public:
    __Renderer_facade(){}
    ~__Renderer_facade() = default;

    void Set_Camera(Camera* cam_){ cam=cam_; }
    void Set_World(Visible* world_){ world=world_; }

    Mat Render() const;
    Color Ray_Color(const Ray&,int current_recur_depth) const;

    void Set_Samples_per_pixel(const int& samples){ samples_per_pixel = samples; }
    void Set_Max_recurrent_depth(const int& max_recur){ max_recurrent_depth = max_recur; }
    
  private:
    Camera* cam;
    Visible* world;

    // Render info
    int samples_per_pixel = 50;
    int max_recurrent_depth = 110;
    bool show_preview_window = true;
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
