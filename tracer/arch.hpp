#ifndef ARCH_H
#define ARCH_H

#include "shape.hpp"
#include "camera.hpp"

#include<sstream>
#include<fstream>
#include<memory>
#include<vector>

class Material;
class Importer{
public:
  Importer(){}
  Importer(std::string filename){
    std::ifstream ifs(filename.c_str(), std::ifstream::in);
    std::stringstream ss;
    if(ifs){ ss<<ifs.rdbuf(); ifs.close(); }
    Import(ss);
  }

  void Import(std::stringstream&);

  std::shared_ptr<Camera> Get_camera(){ return cam; }
  std::vector<std::shared_ptr<Shape>> Get_objects(){ return objects; }
  std::vector<std::shared_ptr<Material>> Get_material(){ return materials; }
    
private:
  std::shared_ptr<Camera> cam;
  std::vector<std::shared_ptr<Shape>> objects;
  std::vector<std::shared_ptr<Material>> materials;
};

#endif
