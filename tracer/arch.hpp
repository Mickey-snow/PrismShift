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
  Importer(std::string filename):file(filename) { Reimport(); }

  void Reimport();

  std::shared_ptr<Camera> Get_camera(){ return cam; }
  
  std::vector<std::shared_ptr<Visible>> Get_objects(){ return objects; }
  std::vector<std::shared_ptr<Material>> Get_material(){ return materials; }
    
private:
  std::string file;
  std::shared_ptr<Camera> cam;
  
  std::vector<std::shared_ptr<Visible>> objects;
  std::vector<std::shared_ptr<Material>> materials;
};

#endif
