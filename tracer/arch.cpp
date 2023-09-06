#include "arch.hpp"
#include<factory.hpp>

#include<exception>
#include<cassert>
#include<fstream>
#include<iostream>
#include<memory>

jsonImporter::jsonImporter(std::string filename) : file(filename) { Import(); }


Json::Value Readjson(std::string file){
  std::ifstream ifs(file.c_str());
  if(!ifs.is_open()) throw std::runtime_error("failed to optn file " + file);
  Json::Value root;  ifs>>root;
  return root;
}



void jsonImporter::Import(){
  std::cout<<"import scene from file: "<<file<<"  ...   "<<std::flush;
  Json::Value root = Readjson(file);

  if(root.isMember("camera"))
    ImportCamera(root["camera"]);
  else{
    std::cout<<"camera not found"<<std::endl;
    camera = nullptr;
  }

  if(root.isMember("materials")) ImportMaterial(root["materials"]);
  else std::cout<<"material not found"<<std::endl;

  scene = std::make_shared<Scene>();
  if(root.isMember("objects")) ImportScene(root["objects"]);
  else std::cout<<"object not found"<<std::endl;

  std::cout<<"Done."<<std::endl;
}

void jsonImporter::ImportMaterial(Json::Value material_arr){
  assert(material_arr.isArray());

  for(const auto& material : material_arr){
    std::string name = material["name"].asString();
    std::string type = material["type"].asString();

    try{
      auto factoryMethod = MaterialFactory::Instance()->GetCreateFn(type);
      this->material[name] = (*factoryMethod)(material["attribute"]);
    } catch(std::runtime_error e){
      std::cerr<<e.what()<<std::endl;
      std::cerr<<"json::importMaterial> at instantiating "<<name<<std::endl;
      throw e;
    }
  }
}

void jsonImporter::ImportScene(Json::Value scene_arr){
  assert(scene_arr.isArray());
  
  for(const auto& obj_attr : scene_arr){
    std::string name = obj_attr["name"].asString();
    std::string type = obj_attr["type"].asString();
    std::shared_ptr<Visible> createdShape;
  
    try{
      auto factoryMethod = ShapeFactory::Instance()->GetCreateFn(type);
      createdShape = (*factoryMethod)(obj_attr["attribute"]);
      if(obj_attr.isMember("material"))
	createdShape->Set_material(this->material[obj_attr["material"].asString()]);
      else std::cout<<"no material for object"<<name<<std::endl;
	
    } catch(std::runtime_error e){
      std::cerr<<e.what()<<std::endl;
      std::cerr<<"json::importScene> at instantiating "<<name<<std::endl;
      throw e;
    }
  
       
    this->scene->Add(createdShape);
  }
}

void jsonImporter::ImportCamera(Json::Value camval){
  Json::RequireMember(camval, "position","lookat","image_height","aspect_ratio","view_angle");
  
  Point3 position,lookat;
  int imageheight;
  double aspectratio, viewangle;
  
  Json::Value vec = camval["position"];
  position = Point3(vec[0].asDouble(), vec[1].asDouble(), vec[2].asDouble());
  vec = camval["lookat"];
  lookat = Point3(vec[0].asDouble(), vec[1].asDouble(), vec[2].asDouble());
  imageheight = camval["image_height"].asInt();
  aspectratio = camval["aspect_ratio"].asDouble();
  viewangle = camval["view_angle"].asDouble();
  
  this->camera = std::make_shared<Camera>(position,lookat,imageheight,aspectratio,viewangle);
}
