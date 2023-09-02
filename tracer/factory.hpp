#ifndef VISUAL_SHAPE_FACTORY_H
#define VISUAL_SHAPE_FACTORY_H

#include<sstream>
#include<memory>
#include<map>
#include<string>
#include<format>

template<typename idtype, typename productType>
class _Factory{
public:
  typedef productType (*Create_Callback)(std::stringstream&);
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


class Shape;
class ShapeFactory{
private:
  class ShapeFactory_ins : public _Factory<std::string, std::shared_ptr<Shape>>{};

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



#endif
