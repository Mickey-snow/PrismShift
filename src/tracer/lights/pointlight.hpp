#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include<util/util.hpp>
#include<material.hpp>
#include<factory.hpp>
#include<bsdf.hpp>

#include<sstream>
#include<string>


class Pointlight : public Material{
public:
  static constexpr std::string name{"pointlight"};

  Pointlight(double r,double g,double b) : Pointlight(Color(r,g,b)) {}
  Pointlight(const Color& col) : color(col) {}

  virtual BSDF CalculateBSDF(const Hit_record&) override{ return {}; }

  Color Emission(const Hit_record&) override{ return color; }
private:
  Color color;
};

namespace{
  std::shared_ptr<Material> CreatePointlight(Json::Value attribute){
    Json::RequireMember(attribute, "rgb");
    
    double r = attribute["rgb"][0].asDouble();
    double g = attribute["rgb"][1].asDouble();
    double b = attribute["rgb"][2].asDouble();
    return std::make_shared<Pointlight>(r,g,b);
  }
  constexpr std::string Pointlight_MaterialID = Pointlight::name;
  const bool pointlight_registered = MaterialFactory::Instance()->Register(Pointlight_MaterialID, CreatePointlight);
}



#endif
