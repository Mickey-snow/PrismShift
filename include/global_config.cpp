#include "global_config.hpp"
#include<args.hpp>

#include<string>
#include<iostream>

void _Global::_Global_ins::Set_config( int argc,  char **argv){
  args::Parser parser(argc, argv);

  for(auto& arg : parser.Parsed()){
    std::string opt = arg.GetOption();
    
    if(opt == "-nw" or
       opt == "--no-window")
      show_render_window = false;

    else if(opt == "-spp" or
	    opt == "--samples-per-pixel")
      samples_per_pixel = std::stoi(arg.GetParameter());

    else if(opt == "-mrd" or
	    opt == "--max-recurrent-depth")
      max_recurrent_depth = std::stoi(arg.GetParameter());

    
    else if(opt == "-nt" or
	    opt == "--no-timer")
      use_timer = false;
    
    else if(opt == "--image-height")
      image_height = std::stoi(arg.GetParameter());

    else if(opt == "-j" or
	    opt == "--threads")
      max_threads = std::stoi(arg.GetParameter());
      
    else
      std::cerr<<"Unknown argument: "<<arg.GetOption()<<' '<<arg.GetParameter()<<std::endl<<std::flush;
  }
}
