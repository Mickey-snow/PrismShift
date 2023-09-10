#include "arch.hpp"
#include<factory.hpp>

#include<exception>
#include<cassert>
#include<fstream>
#include<iostream>
#include<memory>
#include<format>

Json::Value Readjson(std::string file){
  std::ifstream ifs(file.c_str());
  if(!ifs.is_open()) throw std::runtime_error("failed to optn file " + file);
  Json::Value root;  ifs>>root;
  return root;
}
jsonImporter::jsonImporter(std::string filename){
  Json::Value root = Readjson(filename);
  Import(root);
}
jsonImporter::jsonImporter(Json::Value root){ Import(root); }



void jsonImporter::Import(Json::Value root){
  std::cout<<"importing scene   ...   "<<std::flush;

  if(root.isMember("camera"))
    ImportCamera(root["camera"]);
  else{
    std::cout<<"camera not found  "<<std::flush;
    camera = nullptr;
  }

  if(root.isMember("materials")) ImportMaterial(root["materials"]);
  else std::cout<<"material not found  "<<std::flush;

  scene = std::make_shared<Scene>();
  if(root.isMember("objects")) ImportScene(root["objects"]);
  else std::cout<<"object not found  "<<std::flush;

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
	createdShape->Set_Material(this->material[obj_attr["material"].asString()]);
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



objImporter::objImporter(std::string filename){
  bool triangulate = true;
  std::string warn,err;
  const char* basepath = NULL;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn,&err, filename.c_str(), basepath,triangulate);

  if(!warn.empty()) std::cout<<"WARN: "<<warn<<std::endl;
  if(!err.empty()) std::cerr<<"ERR: "<<err<<std::endl;
  if(!ret) throw std::runtime_error("Failed to load from file "+filename);

  Import();
}

void objImporter::Import(){
  jsonimp = new jsonImporter(Makejson());
}



Json::Value objImporter::MakeMaterial(){
  Json::Value root;

  for(size_t i=0;i<materials.size();++i){
    auto nowMat = materials[i];
    Json::Value mat;
    
    mat["name"] = nowMat.name;
    mat["type"] = "phong";
    
    auto& attri = mat["attribute"];
    for(int k=0;k<3;++k){
      attri["ambient"][k] = nowMat.ambient[k];
      attri["diffuse"][k] = nowMat.diffuse[k];
      attri["specular"][k] = nowMat.specular[k];
      attri["emission"][k] = nowMat.emission[k];
    }
    attri["shininess"] = nowMat.shininess;

    root.append(mat);
  }
  
  return root;
}
Json::Value objImporter::MakeObject(){
  Json::Value obj,root;
  
  for(size_t i=0;i<shapes.size();++i){
    size_t index_offset = 0;
    const std::string obj_name = shapes[i].name;
      
    for(size_t f=0;f<shapes[i].mesh.num_face_vertices.size(); ++f){
      size_t fnum = shapes[i].mesh.num_face_vertices[f];
      obj.clear();
      obj["name"] = std::format("mesh{}.face{}",obj_name,f);
      obj["type"] = "triangle";

      const size_t material_id = shapes[i].mesh.material_ids[f];
      const std::string material_name = materials[material_id].name;
      obj["material"] = material_name;
      auto& attr = obj["attribute"];
    
      for(int v=0;v<fnum;++v){
	auto idx = shapes[i].mesh.indices[index_offset+v];

	for(int k=0;k<3;++k){
	  attr["vertex"][v][k] = attrib.vertices[3*size_t(idx.vertex_index)+k];
	  if(idx.normal_index >= 0)
	  attr["normal"][v][k] = attrib.normals[3*idx.normal_index+k];
	}
      }
      root.append(obj);
      index_offset += fnum;
    }
  }
  
  
  return root;
}
Json::Value objImporter::Makejson(){
  Json::Value root;
  root["materials"] = MakeMaterial();
  root["objects"] = MakeObject();
  return root;
}
