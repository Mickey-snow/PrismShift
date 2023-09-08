#ifndef ARCH_H
#define ARCH_H

#include<exception>
#include<string>
#include<memory>
#include<map>

#include<scene.hpp>
#include<camera.hpp>
#include<shape.hpp>
#include<material.hpp>

#include<tiny_obj_loader.h>

class Importer_Base{
public:
  virtual ~Importer_Base() = default;

  virtual std::shared_ptr<Scene> GetScene(void) = 0;
  virtual std::shared_ptr<Camera> GetCamera(void) = 0;
};

namespace Json{ class Value; }
class jsonImporter : public Importer_Base{
public:
  jsonImporter(std::string);
  jsonImporter(Json::Value);

  std::shared_ptr<Scene> GetScene(void) override{ return scene; }
  std::shared_ptr<Camera> GetCamera(void) override{ return camera; }

private:
  std::shared_ptr<Camera> camera;
  std::shared_ptr<Scene> scene;
  std::map<std::string,std::shared_ptr<Material>> material;

  void Import(Json::Value);
  void ImportMaterial(Json::Value);
  void ImportScene(Json::Value);
  void ImportCamera(Json::Value);
};

class objImporter: public Importer_Base{
public:
  objImporter(std::string);

  std::shared_ptr<Scene> GetScene(void) override{ return jsonimp->GetScene(); }
  std::shared_ptr<Camera> GetCamera(void) override{ return jsonimp->GetCamera(); }

private:

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  
  jsonImporter* jsonimp;

  void Import();
  Json::Value Makejson();
  Json::Value MakeObject();
  Json::Value MakeMaterial();
};

class Importer : public Importer_Base{
public:
  Importer(std::string filename):file(filename) { myimporter = nullptr; }
  ~Importer(){ delete myimporter; }

  void SetFile(std::string filename){
    file = filename;
    delete myimporter;
    myimporter = nullptr;
  }
  
  std::shared_ptr<Scene> GetScene(void) override{
    if(!myimporter) InitImporter();
    return myimporter->GetScene();
  }

  std::shared_ptr<Camera> GetCamera(void) override{
    if(!myimporter) InitImporter();
    return myimporter->GetCamera();
  }
  
private:
  std::string file;
  Importer_Base* myimporter;

  void InitImporter(){
    if(file.rfind(".json")==file.length()-5 ||
       file.rfind(".JSON")==file.length()-5){
      myimporter = new jsonImporter(file);
    }
    else if(file.rfind(".obj")==file.length()-4)
      myimporter = new objImporter(file);

    else throw std::runtime_error("file format unrecognized: " + file);
  }
};

#endif
