#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include<material.hpp>
#include<util/util.hpp>
#include<factory.hpp>

#include<string>
#include<sstream>


class Dielectric : public Material{
public:
  static constexpr std::string name{"dielectric"};
  
  Dielectric(const double& index_of_refraction) : eta(index_of_refraction) {}

  virtual Color Ray_Color(const Ray& r, const Hit_record& rec) const override;
  
private:
  double eta;
};



namespace{
  std::shared_ptr<Material> CreateDielectric(std::stringstream& ss){
    double eta; ss>>eta;
    return std::make_shared<Dielectric>(eta);
  }
  constexpr std::string Dielectric_MaterialID = Dielectric::name;
  const bool dielectric_registered = MaterialFactory::Instance()->Register(Dielectric_MaterialID, CreateDielectric);
}



#endif