#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include<util/util.hpp>
#include<material.hpp>
#include<factory.hpp>

#include<string>
#include<sstream>

class Lambertian : public Material{
public:
  static constexpr std::string name{"lambertian"};
  
  Lambertian(const Color& col) : attenuation(col) {}

  virtual Color Ray_Color(const Ray& r,const Hit_record& rec) const override;
  
protected:
  Color attenuation;
};

namespace{
  std::shared_ptr<Material> CreateLambertian(std::stringstream& ss){
    double r,g,b; ss>>r>>g>>b;
    return std::make_shared<Lambertian>(Color(r,g,b));
  }
  constexpr std::string Lambertian_MaterialID = Lambertian::name;
  const bool registered00 = MaterialFactory::Instance()->Register(Lambertian_MaterialID, CreateLambertian);
}

#endif
