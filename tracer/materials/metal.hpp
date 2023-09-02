#ifndef METAL_H
#define METAL_H

#include<material.hpp>
#include<util/util.hpp>
#include<factory.hpp>

#include<string>
#include<sstream>

class Metal : public Material{
public:
  static constexpr std::string name{"metal"};
  
  Metal(const Color& col, const double& f=0) : attenuation(col),fuzz(fabs(f)<1?f:1) {}

  virtual Color Ray_Color(const Ray& r,const Hit_record& rec) const override;
  
protected:
  Color attenuation;
  double fuzz;
};

namespace{
  std::shared_ptr<Material> CreateMetal(std::stringstream& ss){
    double r,g,b,fuzz; ss>>r>>g>>b>>fuzz;
    return std::make_shared<Metal>(Color(r,g,b), fuzz);
  }
  constexpr std::string Metal_MaterialID = Metal::name;
  const bool registered3 = MaterialFactory::Instance()->Register(Metal_MaterialID, CreateMetal);
}


#endif
