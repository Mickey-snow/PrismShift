#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include<util/util.hpp>
#include<material.hpp>
#include<factory.hpp>

#include<sstream>
#include<string>

class Pointlight : public Material{
public:
  static constexpr std::string name{"pointlight"};

  Pointlight(double r,double g,double b) : Pointlight(Color(r,g,b)) {}
  Pointlight(const Color& col) : color(col) {}

  virtual Color Ray_Color(const Ray& r, const Hit_record&) const override;
  
private:
  Color color;
};

namespace{
  std::shared_ptr<Material> CreatePointlight(std::stringstream& ss){
    double r,g,b; ss>>r>>g>>b;
    return std::make_shared<Pointlight>(r,g,b);
  }
  constexpr std::string Pointlight_MaterialID = Pointlight::name;
  const bool pointlight_registered = MaterialFactory::Instance()->Register(Pointlight_MaterialID, CreatePointlight);
}



#endif
