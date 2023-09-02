#include "arch.hpp"
#include "factory.hpp"
#include "util/util.hpp"

#include<sstream>
#include<vector>
#include<memory>

std::shared_ptr<Camera> Import_cam(std::stringstream&);
std::shared_ptr<Visible> Import_object(std::stringstream&);
std::shared_ptr<Material> Import_material(std::stringstream&);

void Importer::Import(std::stringstream& ss){
  cam = Import_cam(ss);
  global_illumin = Import_material(ss);

  int material_count, object_count, link_count;
  ss>>material_count;
  for(int i=0;i<material_count;++i)
    materials.push_back(Import_material(ss));

  ss>>object_count;
  for(int i=0;i<object_count;++i)
    objects.push_back(Import_object(ss));

  ss>>link_count;
  for(int i=0;i<link_count;++i){
    int obj_id, mat_id; ss>>obj_id>>mat_id;
    objects[obj_id]->Set_material(materials[mat_id]);
  }
}

std::shared_ptr<Camera> Import_cam(std::stringstream& ss){
  double x,y,z;
  ss>>x>>y>>z;
  Point3 center_position(x,y,z);

  ss>>x>>y>>z;
  Point3 looking_at(x,y,z);

  int image_height; ss>>image_height;
  double aspect_ratio; ss>>aspect_ratio;
  double view_angle_vertical; ss>>view_angle_vertical;

  return std::make_shared<Camera>(center_position,looking_at, image_height, aspect_ratio, view_angle_vertical);
}

std::shared_ptr<Visible> Import_object(std::stringstream& ss){
  std::string id; ss>>id;
  return (ShapeFactory::Instance()->GetCreateFn(id))(ss);
}

std::shared_ptr<Material> Import_material(std::stringstream& ss){
  std::string id; ss>>id;
  return (MaterialFactory::Instance()->GetCreateFn(id))(ss);
}
