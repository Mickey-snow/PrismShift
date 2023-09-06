#ifndef VISUAL_SHAPE_FACTORY_H
#define VISUAL_SHAPE_FACTORY_H

#include<sstream>
#include<memory>
#include<map>
#include<string>
#include<format>

#include<jsoncpp.h>

template<typename idtype, typename productType>
class _Factory{
public:
  typedef productType (*Create_Callback)(Json::Value);
private:
  using CallbackMap = std::map<idtype, Create_Callback>;
  CallbackMap _callbacks;
    
public:
  // Returns true iff suffessfully register creating function to factory
  bool Register(idtype id, Create_Callback CreateFn){
    return _callbacks.insert(std::pair{id, CreateFn}).second;
  }

  bool isRegistered(idtype id) const{
    return _callbacks.find(id) != _callbacks.end();
  }

  Create_Callback GetCreateFn(idtype shapeid){
    auto it = _callbacks.find(shapeid);
    if(it == _callbacks.end())
      throw std::runtime_error(std::format("Unknown Shape ID: {}", shapeid));
    return it->second;
  }
};


class Visible;
class ShapeFactory{
private:
  class ShapeFactory_ins : public _Factory<std::string, std::shared_ptr<Visible>>{};

public:
  ShapeFactory() = delete;
  ShapeFactory(ShapeFactory&) = delete;
    
  static ShapeFactory_ins* Instance(){
    static ShapeFactory_ins factory;
    return &factory;
  }
};


class Material;
class MaterialFactory{
private:
  class MaterialFactory_ins : public _Factory<std::string, std::shared_ptr<Material>> {};

public:
  MaterialFactory() = delete;
  MaterialFactory(MaterialFactory&) = delete;

  static MaterialFactory_ins* Instance(){
    static MaterialFactory_ins factory;
    return &factory;
  }
};

class Texture;
class TextureFactory{
private:
  class TextureFactory_ins : public _Factory<std::string, std::shared_ptr<Texture>> {};

public:
  TextureFactory() = delete;
  TextureFactory(TextureFactory&) = delete;

  static TextureFactory_ins* Instance(){
    static TextureFactory_ins factory;
    return &factory;
  }
};

#endif
