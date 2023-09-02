#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include<string>

class _Global{
private:
  
  class _Global_ins{
  public:
    bool show_render_window;
    unsigned int samples_per_pixel;
    unsigned int max_recurrent_depth;
    unsigned int image_height;
    std::string output_directory;
    bool use_timer;
    unsigned int max_threads;
    
    _Global_ins(){
      // default setting

      show_render_window = true;
      samples_per_pixel = 20;
      max_recurrent_depth = 65;
      image_height = 530;
      output_directory = "out.ppm";
      use_timer = true;
      max_threads = 25;
    }
   
    void Set_config(int argc, char** argv);
  };

public:
  static _Global_ins* Instance(void){
    static _Global_ins instance;
    return &instance;
  }
};

#endif
